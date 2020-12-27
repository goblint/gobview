[@react.component]
let make = (~syntactic_search: State.syntactic_search, ~dispatch) => {
  let query_string = syntactic_search.query_string;
  let query = syntactic_search.query;
  let query_error = syntactic_search.query_error;
  <div className="card">
    <div className="card-header"> {"Syntactic Search" |> React.string} </div>
    <div className="card-body">
      {switch (syntactic_search.matches) {
       | Some(matches) => <SyntacticSearchResultView matches dispatch />
       | _ =>
         <SyntacticSearchQueryView query_string query query_error dispatch />
       }}
    </div>
  </div>;
};
