module S = State;

[@react.component]
let make = (~matches, ~dispatch) => {
  let clear = _ => {
    dispatch @@ S.Clear_matches;
  };

  <>
    <h5 className="card-title"> {"Results" |> React.string} </h5>
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
          <th scope="col"> {"ID" |> React.string} </th>
        </tr>
      </thead>
      <tbody>
        {matches
         |> List.mapi((i, m) => {
              let (name, loc, signature, id) = m;
              <tr key={string_of_int(i)}>
                <th scope="row"> {i |> string_of_int |> React.string} </th>
                <td> {name |> React.string} </td>
                <td> {signature |> React.string} </td>
                <td>
                  {loc.Cil.file
                   ++ ":"
                   ++ string_of_int(loc.Cil.line)
                   |> React.string}
                </td>
                <td> {id |> string_of_int |> React.string} </td>
              </tr>;
            })
         |> React.list}
      </tbody>
    </table>
  </>;
};
