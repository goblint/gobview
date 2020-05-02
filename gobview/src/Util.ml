open Js_of_ocaml
let log s = Firebug.console##log (Js.string s)
let error s = Firebug.console##error (Js.string s); failwith(s)
let warning s = Firebug.console##warn (Js.string s)
