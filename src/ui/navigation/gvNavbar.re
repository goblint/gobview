open Batteries;

let make_breadcrumb_items = (display: State.display, dispatch) => {
  let on_click = (act, _) => Option.may(dispatch, act);

  <>
    <li className="breadcrumb-item">
      <Link on_click callback_data=`DisplayNothing>
        {"Analysis" |> React.string}
      </Link>
    </li>
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
     }}
  </>;
};

[@react.component]
let make = (~display, ~dispatch) => {
  <nav className="navbar navbar-light bg-light">
    <div className="container-fluid justify-content-start">
      <span className="navbar-brand"> {"Gobview" |> React.string} </span>
      <nav>
        <ol className="breadcrumb my-0">
          {switch (display) {
           | Some(d) => make_breadcrumb_items(d, dispatch)
           | _ =>
             <li className="breadcrumb-item active">
               {"Analysis" |> React.string}
             </li>
           }}
        </ol>
      </nav>
    </div>
  </nav>;
};
