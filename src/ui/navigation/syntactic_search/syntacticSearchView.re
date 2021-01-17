[@react.component]
let make = (~search: State.syntactic_search, ~dispatch) => {
  <div className="card">
    <div className="card-header"> {"Syntactic Search" |> React.string} </div>
    <div className="card-body">
      {switch (search.matches) {
       | Some(matches) => <SyntacticSearchResultView matches dispatch />
       | _ => <SyntacticSearchQueryView search dispatch />
       }}
    </div>
  </div>;
};
