[@react.component]
let make = (~search: State.syntactic_search, ~dispatch) => {
  let kind = search.kind;
  let target = search.target;
  let find = search.find;
  let structure = search.structure;

  let query_json = search.query_json;
  let query = search.query;
  let query_json_error = search.query_json_error;

  let on_click = () => {
    dispatch @@ `ShowSynSearchQueryAsJSON(!search.show_json);
  };

  <>
    {if (search.show_json) {
       <SyntacticSearchQueryEditor
         query_json
         query
         query_json_error
         dispatch
       />;
     } else {
       <SyntacticSearchQueryBuilder kind target find structure dispatch />;
     }}
    <Link on_click>
      <div className="mt-4">
        {"Switch to "
         ++ (
           if (search.show_json) {
             "graphical editor";
           } else {
             "JSON editor";
           }
         )
         |> React.string}
      </div>
    </Link>
  </>;
};
