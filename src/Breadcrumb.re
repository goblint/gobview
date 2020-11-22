open State;

let makeBreadcrumb = (inspect, dispatch) => {
  let onClick = (act, e) => {
    React.Event.Mouse.preventDefault(e);
    dispatch @@ act;
  };

  <>
    <nav>
      <ol className="breadcrumb">
        <li className="breadcrumb-item">
          <a href="#" onClick={onClick(Reset_inspect)}>
            {"Analysis" |> React.string}
          </a>
        </li>
        {switch (inspect) {
         | File(f) =>
           <li className="breadcrumb-item active">
             {f.name |> React.string}
           </li>
         | Func(f) =>
           <>
             <li className="breadcrumb-item">
               <a
                 href="#"
                 onClick={onClick(Inspect_file(f.file_name, f.file_path))}>
                 {f.file_name |> React.string}
               </a>
             </li>
             <li className="breadcrumb-item active">
               {f.name |> React.string}
             </li>
           </>
         }}
      </ol>
    </nav>
  </>;
};

[@react.component]
let make = (~state, ~dispatch) => {
  switch (state.inspect) {
  | Some(i) => makeBreadcrumb(i, dispatch)
  | _ => React.null
  };
};
