open Js_of_ocaml

type options
val options : unit -> options

val autocompleteBS : 
  options ->
  (Dom_html.inputElement Js.t -> unit) [@@js.global "autocompleteBS"]