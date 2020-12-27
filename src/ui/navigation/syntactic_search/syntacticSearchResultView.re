module I = State.Inspect;

let make_result_list = (matches, dispatch) => {
  let clear = _ => {
    dispatch @@ `ClearSearchMatches;
  };

  let on_click = (loc, ev) => {
    React.Event.Mouse.preventDefault(ev);
    dispatch @@ `Inspect_file(I.File.Cil_location(loc));
    dispatch @@ `Set_line(loc.Cil.line);
  };

  <>
    <button
      type_="button" className="btn btn-outline-danger my-2" onClick=clear>
      {"Clear results" |> React.string}
    </button>
    <table className="table table-hover">
      <thead>
        <tr>
          <th scope="col"> {"#" |> React.string} </th>
          <th scope="col"> {"Name" |> React.string} </th>
          <th scope="col"> {"Signature" |> React.string} </th>
          <th scope="col"> {"Location" |> React.string} </th>
        </tr>
      </thead>
      <tbody>
        {matches
         |> List.mapi((i, m) => {
              let (name, loc, signature, _) = m;
              <tr key={string_of_int(i)}>
                <th scope="row"> {i |> string_of_int |> React.string} </th>
                <td> {name |> React.string} </td>
                <td> {signature |> React.string} </td>
                <td>
                  <a href="#" onClick={on_click(loc)}>
                    {loc.Cil.file
                     ++ ":"
                     ++ string_of_int(loc.Cil.line)
                     |> React.string}
                  </a>
                </td>
              </tr>;
            })
         |> React.list}
      </tbody>
    </table>
  </>;
};

[@react.component]
let make = (~matches, ~dispatch) => {
  let onClick = _ => {
    dispatch @@ `ClearSearchMatches;
  };

  <>
    <h5 className="card-title"> {"Results" |> React.string} </h5>
    {if (List.length(matches) > 0) {
       make_result_list(matches, dispatch);
     } else {
       <div className="alert alert-warning alert-dismissible">
         {"No results found" |> React.string}
         <button type_="button" className="btn-close" onClick />
       </div>;
     }}
  </>;
};
