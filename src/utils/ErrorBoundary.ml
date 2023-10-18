open Js_of_ocaml

external%component make: message: Js.js_string Js.t
-> children: React.element list
-> React.element =
{|require("./js/ErrorBoundary").default|}
