[@react.component]
let make = (~state: State.t, ~dispatch) => {
  <>
    <Breadcrumb inspect={state.inspect} dispatch />
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
       | Some(Graph(_)) => <NodeView state dispatch />
       }}
    </div>
  </>;
};
