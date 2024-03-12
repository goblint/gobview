open React.Dom.Dsl.Html;
open Batteries;

let make_breadcrumb_items = (display: State.display, dispatch) => {
  let on_click_func = (f : GvDisplay.func, _) => {
    dispatch @@ `InspectLine(None);
    dispatch @@ `DisplayFile(f.file)
  };
  let on_click_file = (f : GvDisplay.file, _) => {
    if(f.mode == GvDisplay.CIL) {
      dispatch @@ `SwitchToC(f)
      dispatch @@ `InspectLine(None);
    }
  };
  let on_click_cil = (f : GvDisplay.file, _) => {
    if(f.mode == GvDisplay.C) {
      dispatch @@ `SwitchToCil(f);
      dispatch @@ `InspectLine(None);
    }
  };

  <>
    {switch (display) {
    | File(f) =>
      let disButton = "btn btn-secondary btn-sm disabled";
      let actButton = "btn btn-outline-primary btn-sm";
      <div className="d-flex justify-content-between w-100">
        <li className="breadcrumb-item active pt-1"> {f.path |> React.string} </li>
        <div className="btn-group btn-group-toggle">
          <label className={(f.mode==GvDisplay.C)?disButton:actButton} onClick=on_click_file(f)>
            {"C File" |> React.string}
          </label>
          <label className={(f.mode==GvDisplay.CIL)?disButton:actButton} onClick=on_click_cil(f)>
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
