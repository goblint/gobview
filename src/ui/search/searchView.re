open Batteries;

[@react.component]
let make = (~search: Search.t, ~dispatch) => {
  <div className="card">
    <div className="card-header">
      {switch (search.matches) {
        | None => "Search" |> React.string
        | _ => "Results" |> React.string
      }}
      </div>
    <div className="card-body">
      {switch (search.matches) {
       | None => <SearchQueryView search dispatch />
       | matches => <SearchResultView matches dispatch />
       }}
    </div>
  </div>;
};
