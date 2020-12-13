open Js_of_ocaml

type t = DOMParser.t

type source = SString of string

let make () = DOMParser.create ()

let prove _ _ = ()

let parse p s =
  match s with
  | SString s ->
      let doc = DOMParser.parse_from_string p s "application/xml" in
      doc##.documentElement |> Js.Unsafe.coerce
