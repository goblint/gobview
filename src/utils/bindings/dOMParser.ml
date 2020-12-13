open Js_of_ocaml

class type dom_parser =
  object
    method parseFromString :
      Js.js_string Js.t ->
      Js.js_string Js.t ->
      Dom.element Dom.document Js.t Js.meth
  end

let constr = Js.Unsafe.global##._DOMParser

type t = dom_parser Js.t

let create () = new%js constr

let parse_from_string p s m =
  let s = Js.string s in
  let m = Js.string m in
  p##parseFromString s m
