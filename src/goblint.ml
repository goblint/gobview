open Batteries
open Lwt.Infix

type t = {
  input: Lwt_io.input_channel;
  output: Lwt_io.output_channel;
  mutex: Lwt_mutex.t;
  mutable counter: int;
}

let spawn path args =
  let base_args = [| path; "--enable"; "server.enabled"; "--set"; "server.mode"; "unix" |] in
  let cmd = args |> Array.of_list |> Array.append base_args in
  let _proc = Lwt_process.open_process_none (path, cmd) in
  let sock = Lwt_unix.socket PF_UNIX SOCK_STREAM 0 in
  (* Wait a bit for the server to be initialized. *)
  let%lwt () = Lwt_unix.sleep 0.1 in
  let%lwt () = Lwt_unix.connect sock (ADDR_UNIX "goblint.sock") in
  let input = Lwt_io.of_fd ~mode:Lwt_io.Input sock in
  let output = Lwt_io.of_fd ~mode:Lwt_io.Output sock in
  Lwt.return { input; output; mutex = Lwt_mutex.create (); counter = 0 }

let with_lock goblint = Lwt_mutex.with_lock goblint.mutex

let assert_ok (resp: Jsonrpc.Response.t) s = match resp.result with
  | Ok _ -> ()
  | Error _ -> failwith s

let send goblint name params =
  let id = `Int goblint.counter in
  goblint.counter <- goblint.counter + 1;
  let req =
    Jsonrpc.Message.create ?params ~id ~method_:name ()
    |> Jsonrpc.Message.yojson_of_request
    |> Yojson.Safe.to_string in
  let%lwt () = Lwt_io.fprintl goblint.output req in
  let%lwt resp =
    Lwt_io.read_line goblint.input
    >|= Yojson.Safe.from_string
    >|= Jsonrpc.Response.t_of_yojson in
  if resp.id <> id then
    failwith "Response ID doesn't match request ID";
  Lwt.return resp

let ping goblint =
  let ping () =
    let%lwt resp = send goblint "ping" None in
    assert_ok resp "Ping failed";
    Lwt.return_unit
  in with_lock goblint ping

let option_whitelist = [] |> Set.of_list

let config goblint name value =
  if not (Set.mem name option_whitelist) then
    invalid_arg (Printf.sprintf "Option '%s' is not in the whitelist" name);
  let config () =
    let params = `List [`String name; value] in
    let%lwt resp = send goblint "config" (Some params) in
    match resp.result with
    | Ok _ -> Lwt.return_unit
    | Error err -> invalid_arg err.message
  in with_lock goblint config

let analyze ?reanalyze goblint =
  let analyze () =
    let reset = match reanalyze with
      | Some `All -> true
      | _ -> false
    in
    let params = `Assoc [("reset", `Bool reset)] in
    let%lwt resp = send goblint "analyze" (Some params) in
    assert_ok resp "Analysis failed";
    Lwt.return_unit
  in with_lock goblint analyze
