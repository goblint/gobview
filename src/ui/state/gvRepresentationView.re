open React.Dom.Dsl.Html;
open Batteries;

let rec is_simple = r =>
  switch (r) {
  | `List([]) => true
  | `List([r]) => is_simple(r)
  | `Tagged(_, r) => is_simple(r)
  | _ => Representation.is_simple(r)
  };

let rec make_rec = represent =>
  switch (represent) {
  | `Nothing => "No representation available" |> React.string
  | `Value(s) => s |> React.string
  | `Pair(a, b) => make_pair(a, b)
  | `List(l) => make_list(l)
  | `Assoc(l) => make_assoc(l)
  | `Tagged("bot", `Value(s))
  | `Tagged("top", `Value(s)) => make_bot_top(s)
  | `Tagged("set", `List(l)) => make_set(l)
  | `Tagged("intervals", `Pair(`Value(x), `Value(y))) =>
    "[" ++ x ++ ", " ++ y ++ "]" |> React.string
  | `Tagged(_, r) => make_rec(r)
  | _ => "Unexpected value" |> React.string
  }

and make_pair = (a, b) =>
  if (Representation.is_simple(a) && Representation.is_simple(b)) {
    <>
      {"(" |> React.string}
      {make_rec(a)}
      {", " |> React.string}
      {make_rec(b)}
      {")" |> React.string}
    </>;
  } else {
    <ol> <li> {make_rec(a)} </li> <li> {make_rec(b)} </li> </ol>;
  }

and make_assoc = l =>
  <ul>
    ...{l
     |> List.mapi((i, (n, r)) => {
          <li key={string_of_int(i)}>
            {switch (r) {
             | `Nothing => n |> React.string
             | _ as r =>
               if (is_simple(r)) {
                 <>
                   <span style=React.Dom.Style.(make([|font_weight("bold")|]))>
                     {n ++ ": " |> React.string}
                   </span>
                   {make_rec(r)}
                 </>;
               } else {
                 <>
                   <div style=React.Dom.Style.(make([|font_weight("bold")|]))>
                     {n ++ ":" |> React.string}
                   </div>
                   {make_rec(r)}
                 </>;
               }
             }}
          </li>
        })}
  </ul>

and make_list = rs =>
  switch (rs) {
  | [] => "[]" |> React.string
  | [r] => make_rec(r)
  | _ =>
    <ol>
      ...{rs
       |> List.map(make_rec)
       |> List.mapi((i, elt) => {<li key={string_of_int(i)}> elt </li>})}
    </ol>
  }

and make_bot_top = s => {
  let style =
    if (List.mem(s, ["⊥", "⊤"])) {
      React.Dom.Style.make([||]);
    } else {
      React.Dom.Style.(make([|font_style("italic")|]));
    };
  <span style> {s |> React.string} </span>;
}

and make_set = l =>
  switch (l) {
  | [] => "∅" |> React.string
  | _ when List.for_all(is_simple, l) =>
    l
    |> List.map(make_rec)
    |> List.interleave(
         ~first=React.string("{"),
         ~last=React.string("}"),
         React.string(", "),
       )
    |> l => React.Fragment.make(~children=l, ())
  | _ => make_list(l)
  };

[@react.component]
let make = (~represent: Representation.t) => {
  make_rec(represent);
};
