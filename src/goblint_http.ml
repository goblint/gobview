open Batteries
open Cohttp_lwt_unix

let addr = ref "127.0.0.1"
let port = ref 8080
let goblint = ref "goblint"
let rest = ref []

let specs =
  [
    ("-addr", Arg.Set_string addr, "Listen address");
    ("-port", Arg.Set_int port, "Listen port");
    ("-with-goblint", Arg.Set_string goblint, "Path to the Goblint executable");
    ( "-goblint",
      Arg.Rest_all (fun args -> rest := args),
      "Pass the rest of the arguments to Goblint" );
  ]

let paths = ref []

let main () =
  let%lwt gob = Goblint.spawn !goblint (!rest @ !paths) in
  let router = Routes.router gob in
  let callback _ req _ = Router.route router req |> Option.default (Server.respond_not_found ()) in
  let server = Server.make ~callback () in
  Server.create ~mode:(`TCP (`Port !port)) server

let () =
  let program = Sys.argv.(0) in
  let usage = Printf.sprintf "%s [-addr ADDR] [-port PORT] ... path [path ...]" program in
  Arg.parse specs (fun s -> paths := s :: !paths) usage;
  Lwt_main.run (main ())
