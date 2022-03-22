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

let process name body =
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
        let response = R.process body >|= R.yojson_of_response >|= Yojson.Safe.to_string in
        response >>= fun body -> Server.respond_string ~status:`OK ~body ()

let callback _ req body =
  let uri = Request.uri req in
  let path = Uri.path uri in
  let parts = String.split_on_char '/' path |> List.filter (not % String.is_empty) in
  let meth = Request.meth req in
  match meth, parts with
  | `POST, ["api"; name] -> process name body
  | `GET, _ -> Server.respond_not_found () (* TODO: Forward Goblint state *)
  | _ -> Server.respond_not_found ()

let main () =
  let%lwt _ = Goblint.spawn !goblint (!rest @ !paths) in
  let server = Server.make ~callback () in
  Server.create ~mode:(`TCP (`Port !port)) server

let () =
  let program = Sys.argv.(0) in
  let usage = Printf.sprintf "%s [-addr ADDR] [-port PORT] ... path [path ...]" program in
  Arg.parse specs (fun s -> paths := s :: !paths) usage;
  Lwt_main.run (main ())
