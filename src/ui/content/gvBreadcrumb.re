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
        <li className="breadcrumb-item active"> {f.path |> React.string} </li>
        <Button on_click={() => dispatch @@ `DisplayCil(f.path)} type_=`Button>
          ...{"CIL" |> React.string}
        </Button>
      </div>
    | Cil(f) =>
      <div className="d-flex justify-content-between w-100">
        <li className="breadcrumb-item active">
          {f.source_file_path |> React.string}
        </li>
          <Button on_click={() => dispatch @@ `DisplayFile(f.source_file_path)} type_=`Button>
            ...{"C File" |> React.string}
          </Button>
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
