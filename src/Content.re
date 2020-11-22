open State;

[@react.component]
let make = (~state, ~dispatch) => {
  <>
    <Breadcrumb state dispatch />
    {if (Option.is_none(state.inspect)) {
       <Navigation state dispatch />;
     } else {
       React.null;
     }}
  </>;
};
