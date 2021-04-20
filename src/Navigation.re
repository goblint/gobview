[@react.component]
let make = (~state: State.t, ~dispatch) => {
  let search = state.search;
  <>
    <GvFileList cil={state.cil} dispatch />
    <SearchView search dispatch />
  </>;
};
