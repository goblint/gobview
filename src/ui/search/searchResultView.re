open Batteries;

let make_table = (matches, dispatch) => {
  let clear = () => dispatch @@ `ClearSearchMatches;

  let on_click = (loc: GoblintCil.location, _, _) => {
    dispatch @@ `DisplayFile(loc.file);
    dispatch @@ `InspectLine(GvInspect.Line.of_location(loc));
  };

  <>
    <Button class_=["btn", "my-2"] color=`Danger outline=true on_click=clear>
      {"Clear results" |> React.string}
    </Button>
    <CollapsibleList collapsed=false>
      {matches
        |> List.mapi((i,m) => {
          let (name, loc, signature, _) = m;
          let key = string_of_int(i);
            <CollapsibleListItem key=key name=key>
              <table className="table table-hover">
                <thead>
                  <tr>
                    <th scope="col"></th>
                  </tr>
                </thead>
                <tbody>
                  <tr>
                    <th scope="row"> {"Name" |> React.string} </th>
                    <td> {name |> React.string} </td>
                  </tr>
                  <tr>
                    <th scope="row"> {"Signature" |> React.string} </th>
                    <td> {signature |> React.string} </td>
                  </tr>
                  <tr>
                    <th scope="row"> {"Location" |> React.string} </th>
                    <td>
                      <Link on_click={on_click(loc)}>
                        {loc.file
                        ++ ":"
                        ++ string_of_int(loc.line)
                        |> React.string}
                      </Link>
                    </td>
                  </tr>
                </tbody>
              </table>
            </CollapsibleListItem>
        })
        |> React.list
      }
    </CollapsibleList>
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
    {switch (matches) {
     | Loading =>
       <div className="d-flex justify-content-center">
         <div className="spinner-border" />
       </div>
     | NotSupported =>
       <div className="alert alert-warning alert-dismissible">
         {"Query not supported" |> React.string}
         <Button class_=["btn-close"] color=`None on_click>
           React.null
         </Button>
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
