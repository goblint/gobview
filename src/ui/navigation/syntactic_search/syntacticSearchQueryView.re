module S = State.SyntacticSearch;

[@react.component]
let make = (~query_string, ~query, ~query_error, ~dispatch) => {
  let onChange = ev => {
    let v =
      React.Event.Synthetic.target(ev)
      |> Ojs.get(_, "value")
      |> Ojs.string_of_js;
    dispatch @@ `UpdateSearchQuery(v);
  };

  let onClick = ev => {
    React.Event.Synthetic.preventDefault(ev);
    dispatch @@ `ExecuteSearchQuery;
  };

  let string_of_error = e =>
    Option.map(S.Query.string_of_error, e) |> Option.value(~default="");

  let (target, set_target) = React.useState(() => Ok(CodeQuery.Name_t("")));

  React.useEffect1(
    () => {
      switch (target) {
      | Ok(t) =>
        CodeQuery.target_to_yojson(t) |> Yojson.Safe.to_string |> Util.log
      | _ => ()
      };
      None;
    },
    [|target|],
  );

  <>
    <h5 className="card-title"> {"Enter a query" |> React.string} </h5>
    <textarea
      className={
        "form-control" ++ (Option.is_some(query_error) ? " is-invalid" : "")
      }
      rows=10
      value=query_string
      onChange
    />
    <div className="invalid-feedback">
      {"Invalid query: " ++ string_of_error(query_error) |> React.string}
    </div>
    <button
      type_="button"
      className="btn btn-primary mt-3"
      disabled={Option.is_none(query)}
      onClick>
      {"Execute" |> React.string}
    </button>
    <Form on_submit={() => ()}>
      <SyntacticSearchTargetBuilder
        value=target
        on_change={v => set_target(_ => v)}
      />
    </Form>
  </>;
};
