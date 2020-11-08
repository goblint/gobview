val makeProps :
     dot:Js_of_ocaml.Js.js_string Js_of_ocaml.Js.t
  -> unit
  -> < dot:
         Js_of_ocaml.Js.js_string Js_of_ocaml.Js.t Js_of_ocaml.Js.readonly_prop >
     Js_of_ocaml.Js.t
  [@@js.custom
    let makeProps ~dot () = Js_of_ocaml.Js.Unsafe.(obj [|("dot", inject dot)|])]

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
