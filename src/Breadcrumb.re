let make_breadcrumb = (inspect: State.Inspect.t, dispatch) => {
  let on_click = (act, e) => {
    React.Event.Mouse.preventDefault(e);
    dispatch @@ act;
  };

  <nav>
    <ol className="breadcrumb">
      <li className="breadcrumb-item">
        <a href="#" onClick={on_click(`ResetInspect)}>
          {"Analysis" |> React.string}
        </a>
      </li>
      {switch (inspect) {
       | File(f) =>
         <li className="breadcrumb-item active"> {f.name |> React.string} </li>
       | Func(f) =>
         <>
           <li className="breadcrumb-item">
             <a
               href="#"
               onClick={on_click(`InspectFile((f.file_name, f.file_path)))}>
               {f.file_name |> React.string}
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
let make = (~inspect, ~dispatch) => {
  switch (inspect) {
  | Some(i) => make_breadcrumb(i, dispatch)
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
