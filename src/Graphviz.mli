open Js_of_ocaml

val makeProps :
     dot: Js.js_string Js.t
  -> options: <fit: bool Js.t Js.readonly_prop
        ; height: Js.js_string Js.t
        ; width: Js.js_string Js.t
        ; zoom: bool Js.t Js.readonly_prop > Js.t
  -> className: Js.js_string Js.t
  -> unit
  -> < dot: Js.js_string Js.t Js.readonly_prop
        ; options: <fit: bool Js.t Js.readonly_prop
          ; height: Js.js_string Js.t
          ; width: Js.js_string Js.t
          ; zoom: bool Js.t Js.readonly_prop > Js.t Js.readonly_prop
        ; className: Js.js_string Js.t Js.readonly_prop > Js.t
  [@@js.custom
    let makeProps ~dot ~options ~className () =
      object%js
        val dot = dot
        val options = options
        val className = className
      end]

val make :
  < dot: Js.js_string Js.t Js.readonly_prop
    ; options: <fit: bool Js.t Js.readonly_prop
      ; height: Js.js_string Js.t
      ; width: Js.js_string Js.t
      ; zoom: bool Js.t Js.readonly_prop > Js.t Js.readonly_prop
    ; className: Js.js_string Js.t Js.readonly_prop > Js.t
  React.component
  [@@js.custom
    external to_component :
         Ojs.t
      -> < dot: Js.js_string Js.t Js.readonly_prop
            ; options: <fit: bool Js.t Js.readonly_prop
              ; height: Js.js_string Js.t
              ; width: Js.js_string Js.t
              ; zoom: bool Js.t Js.readonly_prop > Js.t Js.readonly_prop
            ; className: Js.js_string Js.t Js.readonly_prop > Js.t
         React.component = "%identity"

    val make_internal : Ojs.t [@@js.global "Graphviz"]

    let make = to_component make_internal]
