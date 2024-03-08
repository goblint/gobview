open React.Dom.Dsl.Html;
open Batteries;
open Syntacticsearch;

module GUi = Search.GraphicalUi;

let options = [
  (Ok(CodeQuery.Name_t("")), "Name"),
  (Error(GUi.ID_t("")), "ID"),
  (Ok(All_t), "All"),
  (Ok(AllGlobVar_t), "All globals"),
  (Ok(Or_t([])), "Or"),
  (Ok(And_t([])), "And"),
];

let compare = (a, b) =>
  switch (a, b) {
  | (Ok(CodeQuery.Name_t(_)), Ok(CodeQuery.Name_t(_)))
  | (Error(GUi.ID_t(_)), Error(GUi.ID_t(_)))
  | (Error(GUi.ID_t(_)), Ok(ID_t(_)))
  | (Ok(All_t), Ok(All_t))
  | (Ok(AllGlobVar_t), Ok(AllGlobVar_t))
  | (Ok(Or_t(_)), Ok(Or_t(_)))
  | (Ok(And_t(_)), Ok(And_t(_))) => true
  | _ => false
  };

[@react.component]
let make = (~value, ~on_change) => {
  let on_select = r => {
    on_change(r);
  };

  let on_change_name = s => {
    on_change(Ok(CodeQuery.Name_t(s)));
  };

  let on_change_id = s => {
    s
    |> int_of_string_opt
    |> Option.map_default(
         i =>
           if (i >= 0) {
             Ok(CodeQuery.ID_t(i));
           } else {
             Error(GUi.ID_t(s));
           },
         Error(GUi.ID_t(s)),
       )
    |> on_change;
  };

  let on_change_or_and = (t, l) => {
    switch (t) {
    | CodeQuery.Or_t(_) => on_change(Ok(Or_t(l)))
    | CodeQuery.And_t(_) => on_change(Ok(And_t(l)))
    | _ => failwith("t should be Or_t(_) or Or_t(_)")
    };
  };

  <>
    <div className="mb-3">
      <Label> ...{"Target" |> React.string} </Label>
      <Select options compare value on_change=on_select />
    </div>
    {switch (value) {
     | Ok(Name_t(s)) =>
       <div className="mb-3"> <Input value=s on_change=on_change_name /> </div>
     | Ok(ID_t(_))
     | Error(GUi.ID_t(_)) =>
       let value =
         switch (value) {
         | Ok(ID_t(i)) => Ok(string_of_int(i))
         | Error(GUi.ID_t(s)) => Error(s)
         | _ => failwith("value should be Ok(ID_t(_)) or Error(GUi.ID_t(_))")
         };
       <div className="mb-3">
         <ValidatedInput value on_change=on_change_id>
           ...{"An ID must be an integer greater than or equal to zero."
            |> React.string}
         </ValidatedInput>
       </div>;
     | Ok(Or_t(l) as t)
     | Ok(And_t(l) as t) =>
       <div className="mb-3">
         <ListInput value=l on_change={on_change_or_and(t)} />
       </div>
     | _ => React.null
     }}
  </>;
};
