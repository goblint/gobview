open Js_of_ocaml

val makeProps :
  children:React.element ->
  message:Js.js_string Js.t ->
  unit ->
  < children : React.element Js.readonly_prop
  ; message : Js.js_string Js.t Js.readonly_prop >
  Js.t
  [@@js.custom
    let makeProps ~children ~message () =
      object%js
        val children = children

        val message = message
      end]

val make :
  < children : React.element Js.readonly_prop
  ; message : Js.js_string Js.t Js.readonly_prop >
  Js.t
  React.component
  [@@js.custom
    external to_component :
      Ojs.t ->
      < children : React.element Js.readonly_prop
      ; message : Js.js_string Js.t Js.readonly_prop >
      Js.t
      React.component = "%identity"

    val make_internal : Ojs.t [@@js.global "ErrorBoundary"]

    let make = to_component make_internal]
