open Js_of_ocaml

external%component make: dot: Js.js_string Js.t
-> options: <fit: bool Js.t Js.readonly_prop
      ; height: Js.js_string Js.t
      ; width: Js.js_string Js.t
      ; zoom: bool Js.t Js.readonly_prop > Js.t
-> className: Js.js_string Js.t
-> React.element =
{|require("graphviz-react").Graphviz|}
