open React.Dom.Dsl.Html;
open Batteries;

[@react.component]
let make = (~results: list((string, Representation.t))) => {
  <ul className="list-group list-group-flush">
    ...{results
     |> List.mapi((i, (n, r)) => {
          <li key={string_of_int(i)} className="list-group-item">
            <div style=React.Dom.Style.(make([|font_weight("bold")|]))>
              {n |> React.string}
            </div>
            <GvRepresentationView represent=r />
          </li>
        })}
  </ul>;
};
