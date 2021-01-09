type kind =
  | Name
  | ID
  | All
  | AllGlobVar
  | Or
  | And;

let kind_of_target = (t: CodeQuery.target) =>
  switch (t) {
  | Name_t(_) => Name
  | ID_t(_) => ID
  | All_t => All
  | AllGlobVar_t => AllGlobVar
  | Or_t(_) => Or
  | And_t(_) => And
  };

let target_of_kind = k =>
  switch (k) {
  | Name => CodeQuery.Name_t("")
  | ID => CodeQuery.ID_t(0)
  | All => CodeQuery.All_t
  | AllGlobVar => CodeQuery.AllGlobVar_t
  | Or => CodeQuery.Or_t([])
  | And => CodeQuery.And_t([])
  };

let options = [
  (Name, "Name"),
  (ID, "ID"),
  (All, "All"),
  (AllGlobVar, "All global variables"),
  (Or, "Or"),
  (And, "And"),
];

type error = {
  kind,
  value: string,
  msg: string,
};

let string_of_error = e => e.msg;

let kind_of_result =
  Result.fold(~ok=t => kind_of_target(t), ~error=e => e.kind);

let from_string = (k, s) => {
  switch (k) {
  | Name => Ok(CodeQuery.Name_t(s))
  | ID =>
    int_of_string_opt(s)
    |> Option.map(i => Ok(CodeQuery.ID_t(i)))
    |> Option.value(
         ~default=Error({kind: ID, value: s, msg: "ID must be an integer"}),
       )
  | Or =>
    String.split_on_char(',', s)
    |> List.map(String.trim)
    |> (l => Ok(CodeQuery.Or_t(l)))
  | And =>
    String.split_on_char(',', s)
    |> List.map(String.trim)
    |> (l => Ok(CodeQuery.And_t(l)))
  | _ => failwith("Unexpected input")
  };
};

let to_string = v =>
  switch (v) {
  | Ok(CodeQuery.Name_t(n)) => n
  | Ok(CodeQuery.ID_t(i)) => string_of_int(i)
  | Ok(CodeQuery.Or_t(o)) => String.concat(", ", o)
  | Ok(CodeQuery.And_t(o)) => String.concat(", ", o)
  | Error({value, _}) => value
  | _ => failwith("Unexpected input")
  };

[@react.component]
let make = (~value: result(CodeQuery.target, error), ~on_change) => {
  let kind = kind_of_result(value);

  let on_select = k => {
    on_change(Ok(target_of_kind(k)));
  };

  let on_change = v => {
    on_change(v);
  };

  <>
    <div>
      <label className="form-label"> {"Target" |> React.string} </label>
      <Select options value=kind on_change=on_select />
    </div>
    {switch (kind) {
     | All
     | AllGlobVar => React.null
     | _ as k =>
       <MappedInput
         value
         from_string={from_string(k)}
         to_string
         on_change
         extras={MappedInput.make_extras()}>
         {switch (value) {
          | Error(e) => string_of_error(e) |> React.string
          | _ => React.null
          }}
       </MappedInput>
     }}
  </>;
};
