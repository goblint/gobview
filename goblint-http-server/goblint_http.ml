open Batteries
open Cohttp_lwt
open Cohttp_lwt_unix
open Lwt.Infix

module Yojson_conv = Ppx_yojson_conv_lib.Yojson_conv

let docroot = ref "run"
let index = ref "index.html"
let addr = ref "127.0.0.1"
let port = ref 8080
let goblint = ref "goblint"
let rest = ref []

let specs =
  [
    ("-docroot", Arg.Set_string docroot, "Serving directory");
    ("-index", Arg.Set_string index, "Name of index file in directory");
    ("-addr", Arg.Set_string addr, "Listen address");
    ("-port", Arg.Set_int port, "Listen port");
    ("-with-goblint", Arg.Set_string goblint, "Path to the Goblint executable");
    ("-goblint", Arg.Rest_all (fun args -> rest := args), "Pass the rest of the arguments to Goblint");
  ]

let paths = ref []

let process state name body =
  match Hashtbl.find_option Api.registry name with
  | None -> Server.respond_not_found ()
  | Some (module R) ->
    let%lwt body = Body.to_string body in
    let body = if body = "" then "null" else body in
    match Yojson.Safe.from_string body with
    | exception Yojson.Json_error err -> Server.respond_error ~status:`Bad_request ~body:err ()
    | json ->
      match R.body_of_yojson json with
      | exception Yojson_conv.Of_yojson_error (exn, _) ->
        Server.respond_error ~status:`Bad_request ~body:(Printexc.to_string exn) ()
      | body ->
        Lwt.catch
          (fun () ->
             R.process state body
             >|= R.yojson_of_response
             >|= Yojson.Safe.to_string
             >>= fun body -> Server.respond_string ~status:`OK ~body ())
          (fun exn -> Server.respond_error ~status:`Bad_request ~body:(Printexc.to_string exn) ())

(* The serving of files is implemented similar as in the binary https://github.com/mirage/ocaml-cohttp/blob/master/cohttp-lwt-unix/bin/cohttp_server_lwt.ml *)
let serve_file ~docroot ~uri =
  let fname = Cohttp.Path.resolve_local_file ~docroot ~uri in
  Server.respond_file ~fname ()

let sort lst =
  let compare_kind = function
    | Some Unix.S_DIR, Some Unix.S_DIR -> 0
    | Some Unix.S_DIR, _ -> -1
    | _, Some Unix.S_DIR -> 1
    | Some Unix.S_REG, Some Unix.S_REG -> 0
    | Some Unix.S_REG, _ -> 1
    | _, Some Unix.S_REG -> -1
    | _, _ -> 0 in
  List.sort
    (fun (ka, a) (kb, b) ->
      let c = compare_kind (ka, kb) in
      if c <> 0 then c
      else String.compare (String.lowercase_ascii a) (String.lowercase_ascii b))
    lst

let html_of_listing uri path listing =
  let li l =
    Printf.sprintf "<li><a href=\"%s\">%s</a></li>" (Uri.to_string l) in
  let html =
    List.map
      (fun (kind, f) ->
        let encoded_f = Uri.pct_encode f in
        match kind with
        | Some Unix.S_DIR ->
            let link = Uri.with_path uri (Filename.concat path (Filename.concat encoded_f "")) in
            li link (Printf.sprintf "<i>%s/</i>" f)
        | Some Unix.S_REG ->
            let link = Uri.with_path uri (Filename.concat path encoded_f) in
            li link f
        | Some _ ->
            Printf.sprintf "<li><s>%s</s></li>" f
        | None -> Printf.sprintf "<li>Error with file: %s</li>" f)
      listing
  in
  let contents = String.concat "\n" html in
  Printf.sprintf
    "<html><body><h2>Directory Listing for <em>%s</em></h2><ul>%s</ul><hr \
      /></body></html>"
    (Uri.pct_decode path) contents

let serve uri path =
  let file_name = Cohttp.Path.resolve_local_file ~docroot:!docroot ~uri in
  Lwt.catch
    (fun () ->
      Lwt_unix.lstat file_name >>= fun stat -> (* for symbolic links lstat returns S_LNK, which will result in a
        forbidden error in this implementation. Use stat instead if symbolic links to folders or files should be handled
        just like folders or files respectively *)
      match stat.Unix.st_kind with
      | Unix.S_DIR -> (
          let path_len = String.length path in
          if path_len <> 0 && path.[path_len - 1] <> '/' then (
            Server.respond_redirect ~uri:(Uri.with_path uri (path ^ "/")) ())
          else (
            match Sys.file_exists (Filename.concat file_name !index) with
            | true -> (
                let uri = Uri.with_path uri (Filename.concat path !index) in
                serve_file ~docroot:!docroot ~uri)
            | false ->
                let%lwt files = Lwt_stream.to_list
                  (Lwt_stream.filter (fun s -> s <> "." && s <> "..") (Lwt_unix.files_of_directory file_name)) in
                let%lwt listing = Lwt_list.map_s (fun f ->
                        let file_name = Filename.concat file_name f in
                        Lwt.try_bind
                          (fun () -> Lwt_unix.LargeFile.stat file_name)
                          (fun stat ->
                            Lwt.return
                              ( Some
                                  stat.Unix.LargeFile.st_kind,
                                f ))
                          (fun _exn -> Lwt.return (None, f))) files in
                let body = html_of_listing uri path (sort listing) in
                Server.respond_string ~status:`OK ~body ()))
      | Unix.S_REG -> serve_file ~docroot:!docroot ~uri
      | _ -> (
          let body = Printf.sprintf "<html><body><h2>Forbidden</h2><p><b>%s</b> is not a normal file or \
          directory</p><hr/></body></html>" path in
          Server.respond_string ~status:`OK ~body ()))
    (function
      | Unix.Unix_error (Unix.ENOENT, "stat", p) as e ->
          if p = file_name then (
            Server.respond_not_found ())
          else Lwt.fail e
      | e -> Lwt.fail e)

let callback state _ req body =
  let uri = Request.uri req in
  let path = Uri.path uri in
  let parts = String.split_on_char '/' path |> List.filter (not % String.is_empty) in
  let meth = Request.meth req in
  match meth, parts with
  | `POST, ["api"; name] -> process state name body
  | `GET, _ -> serve uri path
  | _ -> Server.respond_not_found ()

let main () =
  let%lwt state = Goblint.spawn !goblint (!rest @ !paths) >|= State.make in
  (* run Goblint once with option gobview enabled to copy the index.html and main.js files into the served directory *)
  let%lwt _ = Goblint.analyze ~save_dir:!docroot ~gobview:true state.goblint in
  let callback = callback state in
  let server = Server.make ~callback () in
  Server.create ~mode:(`TCP (`Port !port)) server

let () =
  let program = Sys.argv.(0) in
  let usage = Printf.sprintf "%s [-docroot DOCROOT] [-index INDEX] [-addr ADDR] [-port PORT] ... path [path ...]" program in
  Arg.parse specs (fun s -> paths := s :: !paths) usage;
  Lwt_main.run (main ())
