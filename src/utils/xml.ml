open Js_of_ocaml

type xml = Dom.node Js.t

exception Not_element of xml

exception Not_pcdata of xml

exception No_attribute of string

let to_element (x : xml) =
  match x |> Dom.CoerceTo.element |> Js.Opt.to_option with
  | Some e -> e
  | _ -> raise (Not_element x)

let to_text (x : xml) =
  match x |> Dom.CoerceTo.text |> Js.Opt.to_option with
  | Some e -> e
  | _ -> raise (Not_pcdata x)

let tag (x : xml) =
  let e = to_element x in
  e##.tagName |> Js.to_string

let children (x : xml) =
  let filter_empty_lines n =
    Js.Opt.case (Dom.CoerceTo.text n)
      (fun () -> true)
      (fun t ->
        let data = t##.data |> Js.to_string in
        String.trim data |> String.length |> ( <> ) 0)
  in
  let e = to_element x in
  e##.childNodes |> Dom.list_of_nodeList |> List.filter filter_empty_lines

let pcdata (x : xml) =
  let t = to_text x in
  t##.data |> Js.to_string

let attrib (x : xml) a =
  let e = to_element x in
  let a' = Js.string a in
  match e##getAttribute a' |> Js.Opt.to_option with
  | Some s -> s |> Js.to_string
  | _ -> raise (No_attribute a)

let attribs (x : xml) =
  let e = to_element x in
  let attrs = e##.attributes in
  let rec collect l i =
    if i >= 0 then
      let attr = attrs##item i |> Js.Opt.to_option |> Option.get in
      let n = attr##.name |> Js.to_string in
      let v = attr##.value |> Js.to_string in
      let l' = (n, v) :: l in
      collect l' (i - 1)
    else l
  in
  collect [] (attrs##.length - 1)
