open Batteries

type t = { sock : Lwt_unix.file_descr }

let spawn path args =
  let default = [| path; "--enable"; "server.enabled"; "--set"; "server.mode"; "unix" |] in
  let cmd = args |> Array.of_list |> Array.append default in
  let _proc = Lwt_process.open_process_none (path, cmd) in
  let sock = Lwt_unix.socket PF_UNIX SOCK_STREAM 0 in
  (* Wait a bit for the server to be initialized. *)
  let%lwt _ = Lwt_unix.sleep 0.1 in
  let%lwt _ = Lwt_unix.connect sock (ADDR_UNIX "goblint.sock") in
  Lwt.return { sock }
