module S = State;

let make_breadcrumb = (inspect, dispatch) => {
  let on_click = (act, e) => {
    React.Event.Mouse.preventDefault(e);
    dispatch @@ act;
  };

  <nav>
    <ol className="breadcrumb">
      <li className="breadcrumb-item">
        <a href="#" onClick={on_click(`Reset_inspect)}>
          {"Analysis" |> React.string}
        </a>
      </li>
      {switch (inspect) {
       | S.Inspect.File(f) =>
         <li className="breadcrumb-item active">
           {S.Inspect.File.name(f) |> React.string}
         </li>
       | S.Inspect.Func(f) =>
         <>
           <li className="breadcrumb-item">
             <a
               href="#"
               onClick={on_click(
                 `Inspect_file(
                   S.Inspect.File.Direct_location(
                     S.Inspect.Func.get_file_name(f),
                     S.Inspect.Func.get_file_path(f),
                   ),
                 ),
               )}>
               {S.Inspect.Func.get_file_name(f) |> React.string}
             </a>
           </li>
           <li className="breadcrumb-item active">
             {S.Inspect.Func.get_name(f) |> React.string}
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
