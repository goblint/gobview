let makeFuncList = (Parse.File(funcs, _)) => {
  <ul>
    {funcs
     |> List.mapi((i, func) => {
          let Parse.Funct(_, n) = func;
          <li key={string_of_int(i)}> {n |> React.string} </li>;
        })
     |> React.list}
  </ul>;
};

[@react.component]
let make = (~pdata) => {
  <div className="container-fluid">
    <ul>
      {pdata
       |> Parse.get_files
       |> List.filter(f => !Parse.file_is_empty(f))
       |> List.mapi((i, f) => {
            <li key={string_of_int(i)}>
              {f |> Parse.file_to_path |> React.string}
              {makeFuncList(f)}
            </li>
          })
       |> React.list}
    </ul>
  </div>;
};
