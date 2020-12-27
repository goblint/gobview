[@react.component]
let make = (~syntactic_search, ~dispatch) => {
  let query_string =
    Syntactic_search_state.get_query_string(syntactic_search);
  let query = Syntactic_search_state.get_query(syntactic_search);
  let query_error = Syntactic_search_state.get_query_error(syntactic_search);
  <div className="card">
    <div className="card-header"> {"Syntactic Search" |> React.string} </div>
    <div className="card-body">
      {switch (Syntactic_search_state.get_matches(syntactic_search)) {
       | Some(matches) => <Syntactic_search_result_view matches dispatch />
       | _ =>
         <Syntactic_search_query_view
           query_string
           query
           query_error
           dispatch
         />
       }}
    </div>
  </div>;
};
