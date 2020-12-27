let make_func_list_item = (dispatch, file, i, Parse.Funct(_, func)) => {
  let file_name = Parse.file_to_name(file);
  let file_path = Parse.file_to_path(file);

  let onClick = e => {
    React.Event.Mouse.preventDefault(e);
    dispatch @@ `InspectGraph((func, file_name, file_path));
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
    dispatch @@ `InspectFile((file_name, file_path));
  };

  <li key={string_of_int(i)}>
    <a href="#" onClick> {file_path |> React.string} </a>
    {make_func_list(dispatch, file)}
  </li>;
};

[@react.component]
let make = (~state: State.t, ~dispatch) => {
  let syntactic_search = state.syntactic_search;
  <>
    <ul>
      {state.pdata
       |> Parse.get_files
       |> List.filter(f => !Parse.file_is_empty(f))
       |> List.mapi(make_file_list_item(dispatch))
       |> React.list}
    </ul>
    <SyntacticSearchView syntactic_search dispatch />
  </>;
};
