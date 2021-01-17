[@react.component]
let make = (~syntactic_search: State.syntactic_search, ~dispatch) => {
  let kind = syntactic_search.kind;
  let target = syntactic_search.target;
  let find = syntactic_search.find;
  let structure = syntactic_search.structure;
  let query_string = syntactic_search.query_json;
  let query = syntactic_search.query;
  let query_error = syntactic_search.query_json_error;
  <div className="card">
    <div className="card-header"> {"Syntactic Search" |> React.string} </div>
    <div className="card-body">
      <SyntacticSearchQueryBuilder kind target find structure dispatch />
      {switch (syntactic_search.matches) {
       | Some(matches) => <SyntacticSearchResultView matches dispatch />
       | _ =>
         <SyntacticSearchQueryView query_string query query_error dispatch />
       }}
    </div>
  </div>;
};
