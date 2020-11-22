let makeFuncList = (dispatch, f) => {
  let file_name = Parse.file_to_name(f);
  let file_path = Parse.file_to_path(f);
  let Parse.File(funcs, _) = f;

  let onClick = (func, _) => {
    dispatch @@ State.Inspect_function(func, file_name, file_path);
  };

  <ul>
    {funcs
     |> List.mapi((i, func) => {
          let Parse.Funct(_, n) = func;
          <li key={string_of_int(i)} onClick={onClick(n)}>
            {n |> React.string}
          </li>;
        })
     |> React.list}
  </ul>;
};

[@react.component]
let make = (~dispatch, ~pdata) => {
  <div className="container-fluid">
    <ul>
      {pdata
       |> Parse.get_files
       |> List.filter(f => !Parse.file_is_empty(f))
       |> List.mapi((i, f) => {
            <li key={string_of_int(i)}>
              {f |> Parse.file_to_path |> React.string}
              {makeFuncList(dispatch, f)}
            </li>
          })
       |> React.list}
    </ul>
  </div>;
};
