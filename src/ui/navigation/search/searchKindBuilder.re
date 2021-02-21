open Batteries;

let options = [
  (CodeQuery.Var_k, "Variable"),
  (Fun_k, "Function"),
  (Datatype_k, "Data type"),
];

[@react.component]
let make = (~value, ~on_change) => {
  <div className="mb-3">
    <Label> {"Type" |> React.string} </Label>
    <Select options value on_change />
  </div>;
};
