module SS = State.Syntactic_search;

[@react.component]
let make = (~syntactic_search, ~dispatch) => {
  let query_text = SS.get_query_text(syntactic_search);
  let query = SS.get_query(syntactic_search);
  <Syntactic_search_query_view query_text query dispatch />;
};
