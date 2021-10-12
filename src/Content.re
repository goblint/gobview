open Batteries;

[@react.component]
let make = (~state: State.t, ~dispatch) => {
  let line =
    switch (state.inspect) {
    | Some(Line(_ as l)) => Some(l)
    | _ => None
    };

  <div className="flex-fill overflow-auto">
    <GvBreadcrumb display={state.display} dispatch />
    {switch (state.display) {
     | None => <Navigation state dispatch />
     | Some(File(file)) =>
       <>
         <CodeView
           goblint={state.goblint}
           warnings={state.warnings}
           file
           inspect={state.inspect}
           dispatch
         />
         <GvFileView
           goblint={state.goblint}
           warnings={state.warnings}
           file
           line
           dispatch
         />
       </>
     | Some(Func(func)) => <GvFuncView func dispatch />
     }}
  </div>;
};
