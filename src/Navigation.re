open State;

let make_func_list_item = (dispatch, file, i, Parse.Funct(_, func)) => {
  let file_name = Parse.file_to_name(file);
  let file_path = Parse.file_to_path(file);

  let onClick = e => {
    React.Event.Mouse.preventDefault(e);
    dispatch @@ Inspect_function(func, file_name, file_path);
  };

  <li key={string_of_int(i)}>
    <a href="#" onClick> {func |> React.string} </a>
  </li>;
};

let make_func_list = (dispatch, file) => {
  let Parse.File(funcs, _) = file;
  <ul>
    {funcs |> List.mapi(make_func_list_item(dispatch, file)) |> React.list}
  </ul>;
};

let make_file_list_item = (dispatch, i, file) => {
  let file_name = Parse.file_to_name(file);
  let file_path = Parse.file_to_path(file);

  let onClick = e => {
    React.Event.Mouse.preventDefault(e);
    dispatch @@ Inspect_file(file_name, file_path);
  };

  <li key={string_of_int(i)}>
    <a href="#" onClick> {file_path |> React.string} </a>
    {make_func_list(dispatch, file)}
  </li>;
};

[@react.component]
let make = (~state, ~dispatch) => {
  <>
    <ul>
      {state.pdata
       |> Parse.get_files
       |> List.filter(f => !Parse.file_is_empty(f))
       |> List.mapi(make_file_list_item(dispatch))
       |> React.list}
    </ul>
    {switch (State.cil(state)) {
     | Some(cil) => <Syntactic_search_view cil />
     | _ => React.null
     }}
  </>;
};
