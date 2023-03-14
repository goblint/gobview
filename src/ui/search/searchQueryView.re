open Batteries;

[@react.component]
let make = (~search: Search.t, ~dispatch) => {
  let kind = search.graphical_ui.kind;
  let target = search.graphical_ui.target;
  let find = search.graphical_ui.find;
  let structure = search.graphical_ui.structure;
  let expression = search.graphical_ui.expression;
  let mode = search.graphical_ui.mode;

  let json = search.json_ui.text;
  let query = search.json_ui.query;

  let switch_on_click = (m, _, _) => dispatch @@ `UpdateSearchMode(m);

  switch (search.mode) {
  | Graphical =>
    <>
      <SearchQueryBuilder
        kind
        target
        find
        structure
        expression
        mode
        dispatch
      />
      <Link on_click={switch_on_click(Search.Json)}>
        <div className="mt-4"> {"Switch to JSON editor" |> React.string} </div>
      </Link>
    </>
  | Json =>
    <>
      <SearchQueryEditor json query dispatch />
      <Link on_click={switch_on_click(Search.Graphical)}>
        <div className="mt-4">
          {"Switch to graphical editor" |> React.string}
        </div>
      </Link>
    </>
  };
};
