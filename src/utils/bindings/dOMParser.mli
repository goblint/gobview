open Js_of_ocaml

type t

val create : unit -> t

val parse_from_string : t -> string -> string -> Dom.element Dom.document Js.t
