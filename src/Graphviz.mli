val makeProps :
     dot:Js_of_ocaml.Js.js_string Js_of_ocaml.Js.t
  -> unit
  -> < dot:
         Js_of_ocaml.Js.js_string Js_of_ocaml.Js.t Js_of_ocaml.Js.readonly_prop >
     Js_of_ocaml.Js.t
  [@@js.custom
    let makeProps ~dot () =
      object%js
        val dot = dot
      end]

val make :
  < dot: Js_of_ocaml.Js.js_string Js_of_ocaml.Js.t Js_of_ocaml.Js.readonly_prop >
  Js_of_ocaml.Js.t
  React.component
  [@@js.custom
    external to_component :
         Ojs.t
      -> < dot:
             Js_of_ocaml.Js.js_string Js_of_ocaml.Js.t
             Js_of_ocaml.Js.readonly_prop >
         Js_of_ocaml.Js.t
         React.component = "%identity"

    val makeInternal : Ojs.t [@@js.global "Graphviz"]

    let make = to_component makeInternal]
