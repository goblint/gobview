open Batteries;

let make_table = (matches, dispatch) => {
  let clear = () => dispatch @@ `ClearSearchMatches;

  let on_click = (loc: Cil.location, _, _) => {
    dispatch @@ `DisplayFile(loc.file);
    dispatch @@ `InspectLine(GvInspect.Line.of_location(loc));
  };

  <>
    <Button class_=["btn", "my-2"] color=`Danger outline=true on_click=clear>
      {"Clear results" |> React.string}
    </Button>
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
              let key = string_of_int(i);
              <tr key>
                <th scope="row"> {key |> React.string} </th>
                <td> {name |> React.string} </td>
                <td> {signature |> React.string} </td>
                <td>
                  <Link on_click={on_click(loc)}>
                    {loc.file
                     ++ ":"
                     ++ string_of_int(loc.line)
                     |> React.string}
                  </Link>
                </td>
              </tr>;
            })
         |> React.list}
      </tbody>
    </table>
  </>;
};

[@react.component]
let make = (~matches: Search.matches, ~dispatch) => {
  React.useEffect1(
    () => {
      if (matches == Loading) {
        dispatch @@ `PerformSearch;
      };
      None;
    },
    [|matches|],
  );

  let on_click = _ => {
    dispatch @@ `ClearSearchMatches;
  };

  <>
    <h5 className="card-title"> {"Results" |> React.string} </h5>
    {switch (matches) {
     | Loading =>
       <div className="d-flex justify-content-center">
         <div className="spinner-border" />
       </div>
     | Done([]) =>
       <div className="alert alert-warning alert-dismissible">
         {"No results found" |> React.string}
         <Button class_=["btn-close"] color=`None on_click>
           React.null
         </Button>
       </div>
     | Done(matches) => make_table(matches, dispatch)
     | _ => React.null
     }}
  </>;
};
