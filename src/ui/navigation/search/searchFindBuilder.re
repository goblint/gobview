open Batteries;

let options = [
  (CodeQuery.Uses_f, "Uses"),
  (Decl_f, "Declarations"),
  (Defs_f, "Definitions"),
  (UsesWithVar_f(""), "Uses with variable"),
  (Returns_f, "Returns"),
];

let compare = (a, b) =>
  switch (a, b) {
  | (CodeQuery.UsesWithVar_f(_), CodeQuery.UsesWithVar_f(_)) => true
  | (a, b) => a == b
  };

[@react.component]
let make = (~value, ~on_change) => {
  let on_select = f => {
    on_change(f);
  };

  let on_change_uses_with_var = s => {
    on_change(CodeQuery.UsesWithVar_f(s));
  };

  <>
    <div className="mb-3">
      <Label> {"Find" |> React.string} </Label>
      <Select options compare value on_change=on_select />
    </div>
    {switch (value) {
     | UsesWithVar_f(s) =>
       <div className="mb-3">
         <Input value=s on_change=on_change_uses_with_var />
       </div>
     | _ => React.null
     }}
  </>;
};
