open Js_of_ocaml
let log s = Firebug.console##log (Js.string s)
let error_without_fail s = Firebug.console##error (Js.string s)
let error s = error_without_fail(s); failwith(s)
let warning s = Firebug.console##warn (Js.string s)
