open State;

[@react.component]
let make = (~state, ~dispatch) => {
  <>
    <Breadcrumb inspect={State.inspect_opt(state)} dispatch />
    <div className="flex-fill overflow-auto">
      {switch (state.inspect) {
       | None => <Navigation state dispatch />
       | Some(File(_)) =>
         <CodeView
           state
           dispatch
           calls={state.pdata |> Parse.get_calls}
           warnings={state.pdata |> Parse.get_warnings}
         />
       | Some(Func(_)) => <NodeView state dispatch />
       }}
    </div>
  </>;
};
