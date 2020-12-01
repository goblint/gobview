open State;

let makeBreadcrumb = (inspect, dispatch) => {
  let onClick = (act, e) => {
    React.Event.Mouse.preventDefault(e);
    dispatch @@ act;
  };

  <nav>
    <ol className="breadcrumb">
      <li className="breadcrumb-item">
        <a href="#" onClick={onClick(Reset_inspect)}>
          {"Analysis" |> React.string}
        </a>
      </li>
      {switch (inspect) {
       | Inspect.File(f) =>
         <li className="breadcrumb-item active">
           {Inspect.File.name(f) |> React.string}
         </li>
       | Inspect.Func(f) =>
         <>
           <li className="breadcrumb-item">
             <a
               href="#"
               onClick={onClick(
                 Inspect_file(
                   Inspect.Func.file_name(f),
                   Inspect.Func.file_path(f),
                 ),
               )}>
               {Inspect.Func.file_name(f) |> React.string}
             </a>
           </li>
           <li className="breadcrumb-item active">
             {Inspect.Func.name(f) |> React.string}
           </li>
         </>
       }}
    </ol>
  </nav>;
};

[@react.component]
let make = (~inspect, ~dispatch) => {
  switch (inspect) {
  | Some(i) => makeBreadcrumb(i, dispatch)
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
