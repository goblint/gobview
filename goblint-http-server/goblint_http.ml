open Batteries
open Cohttp_lwt
open Cohttp_lwt_unix
open Lwt.Infix

module Yojson_conv = Ppx_yojson_conv_lib.Yojson_conv

let addr = ref "127.0.0.1"
let port = ref 8080
let goblint = ref "goblint"
let rest = ref []

let specs =
  [
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

let forward (state: State.t) path = match state.save_run with
  | Some save_run ->
    let fname = Filename.concat save_run path in
    Server.respond_file ~fname ()
  | None -> Server.respond_not_found ()

let callback state _ req body =
  let uri = Request.uri req in
  let path = Uri.path uri in
  let parts = String.split_on_char '/' path |> List.filter (not % String.is_empty) in
  let meth = Request.meth req in
  match meth, parts with
  | `POST, ["api"; name] -> process state name body
  | `GET, _ -> forward state path
  | _ -> Server.respond_not_found ()

let main () =
  let%lwt state = Goblint.spawn !goblint (!rest @ !paths) >|= State.make in
  let callback = callback state in
  let server = Server.make ~callback () in
  Server.create ~mode:(`TCP (`Port !port)) server

let () =
  let program = Sys.argv.(0) in
  let usage = Printf.sprintf "%s [-addr ADDR] [-port PORT] ... path [path ...]" program in
  Arg.parse specs (fun s -> paths := s :: !paths) usage;
  Lwt_main.run (main ())
