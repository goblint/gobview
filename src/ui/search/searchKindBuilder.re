open React.Dom.Dsl.Html;
open Batteries;
open Syntacticsearch;

let options = [
  (CodeQuery.Var_k, "Variable"),
  (Fun_k, "Function"),
  (Datatype_k, "Data type"),
];

[@react.component]
let make = (~value, ~on_change) => {
  <div className="mb-3">
    <Label> ...{"Type" |> React.string} </Label>
    <Select options value on_change />
  </div>;
};
