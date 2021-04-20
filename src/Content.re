open Batteries;

[@react.component]
let make = (~state: State.t, ~dispatch) => {
  <div className="flex-fill overflow-auto">
    {switch (state.display) {
     | None => <Navigation state dispatch />
     | Some(File(file)) =>
       <CodeView
         goblint={state.goblint}
         warnings={state.warnings}
         file
         inspect={state.inspect}
         dispatch
       />
     | Some(Func(func)) => <GvFuncView func dispatch />
     }}
  </div>;
};
