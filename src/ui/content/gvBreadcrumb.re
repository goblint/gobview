open Batteries;

let make_breadcrumb_items = (display: State.display, dispatch) => {
  let on_click = (act, _) => Option.may(dispatch, act);

  <>
    {switch (display) {
     | File(f) =>
       <li className="breadcrumb-item active"> {f.path |> React.string} </li>
     | Func(f) =>
       <>
         <li className="breadcrumb-item">
           <Link on_click callback_data={`DisplayFile(f.file)}>
             {f.file |> React.string}
           </Link>
         </li>
         <li className="breadcrumb-item active"> {f.name |> React.string} </li>
       </>
       // Remove breadcrumbs if the callgraph is displayed
      | Graph(_) => <div/>
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
