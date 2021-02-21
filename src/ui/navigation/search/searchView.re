open Batteries;

[@react.component]
let make = (~search: Search.t, ~dispatch) => {
  <div className="card">
    <div className="card-header"> {"Search" |> React.string} </div>
    <div className="card-body">
      {switch (search.matches) {
       | Some(matches) => <SearchResultView matches dispatch />
       | _ => <SearchQueryView search dispatch />
       }}
    </div>
  </div>;
};
