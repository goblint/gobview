open Batteries;

let make_active_tab = name => {
  <li className="nav-item">
    <Link class_=["nav-link", "active"]> {name |> React.string} </Link>
  </li>;
};

let make_tabs = (display: GvDisplay.t, dispatch) => {
  let on_click = (act, _) => Option.may(dispatch, act);

  <ul className="nav nav-tabs">
    {switch (display) {
     | Graph((_, Some(File(file)))) =>
       <>
         <li className="nav-item">
           <Link
             class_=["nav-link"]
             on_click
             callback_data={`DisplayFile(file.path)}>
             {"Code" |> React.string}
           </Link>
         </li>
         {make_active_tab("Graph")}
       </>

     | Graph((_, Some(Func(func)))) =>
       <>
         <li className="nav-item">
           <Link
             class_=["nav-link"]
             on_click
             callback_data={`DisplayFunc((func.name, func.file))}>
             {"Code" |> React.string}
           </Link>
         </li>
           {make_active_tab("Graph")}
       </>
     | Graph((_, None)) =>
       <>
         <li className="nav-item">
           <Link
             class_=["nav-link"] on_click callback_data={`DisplayNothing}>
             {"Code" |> React.string}
           </Link>
         </li>
           {make_active_tab("Graph")}
       </>
     | _ =>
       <>
         {make_active_tab("Code")}
         <Link
           class_=["nav-link"] on_click callback_data={`DisplayUsageGraph(None)}>
           {"Graph" |> React.string}
         </Link>
       </>
     }}
  </ul>;
};
[@react.component]
let make = (~state: State.t, ~display: GvDisplay.t, ~dispatch, ~graph) => {
  let line =
    switch (state.inspect) {
    | Some(Line(_ as l)) => Some(l)
    | _ => None
    };

  <div className="content d-flex flex-column h-75 p-4">
    {make_tabs(display, dispatch)}
    {switch (display) {
     | File(file) =>
       <>
         <GvBreadcrumb display={state.display} dispatch />
         <GvFileView
           goblint={state.goblint}
           warnings={state.warnings}
           file
           line
           dispatch
         />
       </>
     | Func(func) =>
       <>
         <GvBreadcrumb display={state.display} dispatch />
         <GvFuncView func dispatch />
       </>
     | Graph((ctx, _)) => <GvUsageGraphView graph ctx dispatch />
     }}
  </div>;
};
