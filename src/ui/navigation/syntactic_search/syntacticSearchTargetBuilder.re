type error =
  | ID(string); // This happens when we cannot convert the input into an integer

let options = [
  (Ok(CodeQuery.Name_t("")), "Name"),
  (Error(ID("")), "ID"),
  (Ok(All_t), "All"),
  (Ok(AllGlobVar_t), "All globals"),
  (Ok(Or_t([])), "Or"),
  (Ok(And_t([])), "And"),
];

let compare = (a, b) =>
  switch (a, b) {
  | (Ok(CodeQuery.Name_t(_)), Ok(CodeQuery.Name_t(_)))
  | (Error(ID(_)), Error(ID(_)))
  | (Error(ID(_)), Ok(ID_t(_)))
  | (Ok(All_t), Ok(All_t))
  | (Ok(AllGlobVar_t), Ok(AllGlobVar_t))
  | (Ok(Or_t(_)), Ok(Or_t(_)))
  | (Ok(And_t(_)), Ok(And_t(_))) => true
  | _ => false
  };

[@react.component]
let make = (~value, ~on_change) => {
  let on_select = k => {
    on_change(k);
  };

  let on_change_name = n => {
    on_change(Ok(CodeQuery.Name_t(n)));
  };

  let on_change_id = s => {
    s
    |> int_of_string_opt
    |> Option.fold(~none=Error(ID(s)), ~some=i =>
         if (i >= 0) {
           Ok(CodeQuery.ID_t(i));
         } else {
           Error(ID(s));
         }
       )
    |> on_change;
  };

  let on_change_or_and = (t, l) => {
    switch (t) {
    | CodeQuery.Or_t(_) => on_change(Ok(Or_t(l)))
    | CodeQuery.And_t(_) => on_change(Ok(And_t(l)))
    | _ => ()
    };
  };

  <>
    <div>
      <label className="form-label"> {"Target" |> React.string} </label>
      <Select options compare value on_change=on_select />
    </div>
    {switch (value) {
     | Ok(Name_t(n)) =>
       <div className="mt-2"> <Input value=n on_change=on_change_name /> </div>
     | Ok(ID_t(_))
     | Error(ID(_)) =>
       let value =
         switch (value) {
         | Ok(ID_t(i)) => Ok(string_of_int(i))
         | Error(ID(s)) => Error(s)
         | _ => Error("") // This should never happen
         };
       <div className="mt-2">
         <ValidatedInput value on_change=on_change_id>
           {"An ID must be an integer greater than or equal to zero."
            |> React.string}
         </ValidatedInput>
       </div>;
     | Ok(Or_t(l) as t)
     | Ok(And_t(l) as t) =>
       <div className="mt-2">
         <ListInput value=l on_change={on_change_or_and(t)} />
       </div>
     | _ => React.null
     }}
  </>;
};
