let options = [
  (CodeQuery.None_s, "None"),
  (Fun_s(""), "Function"),
  (Cond_s, "Condition"),
  (NonCond_s, "Non-condition"),
];

let compare = (a, b) =>
  switch (a, b) {
  | (CodeQuery.Fun_s(_), CodeQuery.Fun_s(_)) => true
  | (a, b) => a == b
  };

[@react.component]
let make = (~value, ~on_change) => {
  let on_select = s => {
    on_change(s);
  };

  let on_change_fun = s => {
    on_change(CodeQuery.Fun_s(s));
  };

  <>
    <div>
      <label className="form-label"> {"Structure" |> React.string} </label>
      <Select options compare value on_change=on_select />
    </div>
    {switch (value) {
     | Fun_s(s) =>
       <div className="mt-2"> <Input value=s on_change=on_change_fun /> </div>
     | _ => React.null
     }}
  </>;
};
