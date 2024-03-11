open React.Dom.Dsl.Html;
open Batteries;

let make_breadcrumb_items = (display: State.display, dispatch) => {
  let on_click = (f : GvDisplay.func, _) => {
    dispatch @@ `InspectLine(None);
    dispatch @@ `DisplayFile(f.file)
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
          <label className="btn btn-outline-primary btn-sm" onClick={(_) => dispatch @@ `DisplayCil(f.path)}>
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
          <label className="btn btn-outline-primary btn-sm" onClick={(_) => dispatch @@ `DisplayFile(f.source_file_path)}>
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
           <Link on_click={on_click(f)}>
             ...{f.file |> React.string}
           </Link>
         </li>
         <li className="breadcrumb-item active"> {f.name |> React.string} </li>
       </>
     }}
  </>;
};

[@react.component]
let make = (~display, ~dispatch) => {
  <nav >
    <ol className="breadcrumb">
      {switch (display) {
       | Some(d) => make_breadcrumb_items(d, dispatch)
       | _ => <div/>
       }}
    </ol>
  </nav>;
};
