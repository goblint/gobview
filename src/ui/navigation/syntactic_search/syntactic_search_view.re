module SS = State.Syntactic_search;

[@react.component]
let make = (~syntactic_search, ~dispatch) => {
  let query_text = SS.get_query_text(syntactic_search);
  let query = SS.get_query(syntactic_search);
  <div className="card">
    <div className="card-header"> {"Syntactic Search" |> React.string} </div>
    <div className="card-body">
      {switch (SS.get_matches(syntactic_search)) {
       | Some(matches) => <Syntactic_search_result_view matches dispatch />
       | _ => <Syntactic_search_query_view query_text query dispatch />
       }}
    </div>
  </div>;
};
