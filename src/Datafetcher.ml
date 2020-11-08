open Js_of_ocaml
open Js_of_ocaml_lwt
open Lwt
(* open Js *)

let base_url = "http://localhost:8000/"

let http_get url =
  XmlHttpRequest.get url
  >>= fun r ->
  let cod = r.XmlHttpRequest.code in
  let msg = r.XmlHttpRequest.content in
  if cod = 0 || cod = 200 then Lwt.return msg else fst (Lwt.wait ())

let http_get_with_base filename = http_get @@ base_url^filename

let get_file f =
  try Lwt.return (Sys_js.read_file ~name:f) with Not_found -> http_get f >|= fun s -> s