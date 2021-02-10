open Js_of_ocaml
open Js_of_ocaml_lwt
open Lwt

type error = HttpFailure of int | NoContentAvailable

let get url =
  XmlHttpRequest.perform_raw ~response_type:XmlHttpRequest.ArrayBuffer url
  >>= fun r ->
  let code = r.code in
  if code < 200 || code >= 400 then Lwt.return (Error (HttpFailure code))
  else
    Js.Opt.case r.content
      (fun () -> Lwt.return (Error NoContentAvailable))
      (fun c -> Lwt.return (Ok (Typed_array.String.of_arrayBuffer c)))

let get_opt url = get url >>= fun resp -> Lwt.return (Result.to_option resp)
