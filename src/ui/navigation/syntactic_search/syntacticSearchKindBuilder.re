let options = [
  (CodeQuery.Var_k, "Variable"),
  (Fun_k, "Function"),
  (Datatype_k, "Data type"),
];

[@react.component]
let make = (~value, ~on_change) => {
  let on_select = k => {
    on_change(k);
  };

  <div>
    <label className="form-label"> {"Type" |> React.string} </label>
    <Select options value on_change=on_select />
  </div>;
};
