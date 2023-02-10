open Batteries;

[@react.component]
let make = (~state: State.t, ~display: GvDisplay.t, ~dispatch) => {
  let line =
    switch (state.inspect) {
    | Some(Line(_ as l)) => Some(l)
    | _ => None
    };

  <div className="d-flex flex-column h-75 overflow-auto p-4">
    <GvBreadcrumb display={state.display} dispatch />
    {switch display {
     | File(file) =>
       <GvFileView
         goblint={state.goblint}
         warnings={state.warnings}
         file
         line
         dispatch
       />
     | Func(func) => <GvFuncView func dispatch />
     }}
  </div>;
};
