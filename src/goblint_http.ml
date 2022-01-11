open Batteries

let addr = ref "127.0.0.1"
let port = ref 8080
let goblint = ref "goblint"
let rest = ref []

let specs = [
  ("-addr", Arg.Set_string addr, "Listen address");
  ("-port", Arg.Set_int port, "Listen port");
  ("-with-goblint", Arg.Set_string goblint, "Path to the Goblint executable");
  ("-goblint", Arg.Rest_all (fun args -> rest := args), "Pass the rest of the arguments to Goblint")
]

let paths = ref []

let () =
  let program = Sys.argv.(0) in
  let usage = Printf.sprintf "%s [-addr ADDR] [-port PORT] ... path [path ...]" program in
  Arg.parse specs (fun s -> paths := s :: !paths) usage
