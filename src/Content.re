open Batteries;

[@react.component]
let make = (~state: State.t, ~dispatch) => {
  let display = state.display;
  <>
    <Breadcrumb display dispatch />
    <div className="flex-fill overflow-auto">
      {switch (state.display) {
       | None => <Navigation state dispatch />
       | Some(File(f)) =>
         <CodeView
           state
           code={Option.default("", f.contents)}
           dispatch
           calls={state.pdata |> Parse.get_calls}
           warnings={state.pdata |> Parse.get_warnings}
         />
       | Some(Func(func)) => <NodeView func dispatch />
       }}
    </div>
  </>;
};
