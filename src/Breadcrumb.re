let make_breadcrumb = (display: State.display, dispatch) => {
  let on_click = (act, e) => {
    React.Event.Mouse.preventDefault(e);
    dispatch @@ act;
  };

  <nav>
    <ol className="breadcrumb">
      <li className="breadcrumb-item">
        <a href="#" onClick={on_click(`DisplayNothing)}>
          {"Analysis" |> React.string}
        </a>
      </li>
      {switch (display) {
       | File(f) =>
         <li className="breadcrumb-item active"> {f.path |> React.string} </li>
       | Func(f) =>
         <>
           <li className="breadcrumb-item">
             <a href="#" onClick={on_click(`DisplayFile(f.file))}>
               {f.file |> React.string}
             </a>
           </li>
           <li className="breadcrumb-item active">
             {f.name |> React.string}
           </li>
         </>
       }}
    </ol>
  </nav>;
};

[@react.component]
let make = (~display, ~dispatch) => {
  switch (display) {
  | Some(d) => make_breadcrumb(d, dispatch)
  | _ =>
    <nav>
      <ol className="breadcrumb">
        <li className="breadcrumb-item active">
          {"Analysis" |> React.string}
        </li>
      </ol>
    </nav>
  };
};
