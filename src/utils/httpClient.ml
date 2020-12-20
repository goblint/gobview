open Js_of_ocaml
open Js_of_ocaml_lwt
open Lwt

let get_opt url =
  XmlHttpRequest.perform_raw ~response_type:XmlHttpRequest.ArrayBuffer url
  >>= fun r ->
  let code = r.code in
  if code <> 200 then Lwt.return_none
  else
    r.content |> Js.Opt.to_option
    |> Option.map Typed_array.String.of_arrayBuffer
    |> Lwt.return

let get url =
  get_opt url >>= function
  | Some r -> Lwt.return r
  | None -> Lwt.fail_with ("Cannot fetch resource: " ^ url)
