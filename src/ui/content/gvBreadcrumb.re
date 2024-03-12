open React.Dom.Dsl.Html;
open Batteries;
open GoblintCil;

let make_breadcrumb_items = (display: State.display, dispatch, cilAST) => {
  let on_click_func = (f : GvDisplay.func, _) => {
    dispatch @@ `InspectLine(None);
    dispatch @@ `DisplayFile(f.file)
  };
  let on_click_file = (f : GvDisplay.cilfile, _) => {
    dispatch @@ `InspectLine(None);
    dispatch @@ `DisplayFile(f.source_file_path)
  };
  let on_click_cil = (f : GvDisplay.file, _) => {
    if(Option.is_none(f.cil)) {
      let global_to_string = (g) => Pretty.sprint(~width=100, (printGlobal(defaultCilPrinter, (), g)));
      let cil = foldGlobals(cilAST, (acc, g) => {
        if(String.equal(f.path, get_globalLoc(g).file)) {
          acc ++ global_to_string(g);
        } else {
          acc;
        }}, "");
      dispatch @@ `DisplayCil(f.path, cil);
    } else {
      dispatch @@ `DisplayCil(f.path, Option.get(f.cil));
    }
    dispatch @@ `InspectLine(None);
  };

  <>
    {switch (display) {
    | File(f) =>
      <div className="d-flex justify-content-between w-100">
        <li className="breadcrumb-item active pt-1"> {f.path |> React.string} </li>
        <div className="btn-group btn-group-toggle">
          <label className="btn btn-secondary btn-sm disabled">
            {"C File" |> React.string}
          </label>
          <label className="btn btn-outline-primary btn-sm" onClick=on_click_cil(f)>
            {"CIL" |> React.string}
          </label>
        </div>
      </div>
    | Cil(f) =>
      <div className="d-flex justify-content-between w-100">
        <li className="breadcrumb-item active pt-1">
          {f.source_file_path |> React.string}
        </li>
        <div className="btn-group btn-group-toggle">
          <label className="btn btn-outline-primary btn-sm" onClick=on_click_file(f)>
            {"C File" |> React.string}
          </label>
          <label className="btn btn-secondary btn-sm disabled">
            {"CIL" |> React.string}
          </label>
        </div>
      </div>
     | Func(f) =>
       <>
         <li className="breadcrumb-item">
           <Link on_click={on_click_func(f)}>
             ...{f.file |> React.string}
           </Link>
         </li>
         <li className="breadcrumb-item active"> {f.name |> React.string} </li>
       </>
     }}
  </>;
};

[@react.component]
let make = (~display, ~dispatch, ~cilAST) => {
  <nav >
    <ol className="breadcrumb">
      {switch (display) {
       | Some(d) => make_breadcrumb_items(d, dispatch, cilAST)
       | _ => <div/>
       }}
    </ol>
  </nav>;
};
