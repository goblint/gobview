module S = State;
module SS = S.Syntactic_search;

[@react.component]
let make = (~query_text, ~query, ~dispatch) => {
  let onChange = ev => {
    let v =
      React.Event.Synthetic.target(ev)
      |> Ojs.get(_, "value")
      |> Ojs.string_of_js;
    dispatch @@ S.Update_query(v);
  };

  let onClick = ev => {
    React.Event.Synthetic.preventDefault(ev);
    dispatch @@ S.Execute_query;
  };

  let is_error = q =>
    Option.map(Result.is_error, q) |> Option.value(~default=false);

  let get_error = q =>
    switch (q) {
    | Some(Error(e)) => SS.string_of_error(e)
    | _ => ""
    };

  <>
    <h5 className="card-title"> {"Enter a query" |> React.string} </h5>
    <textarea
      id="syntacticSearchQueryInput"
      className={"form-control" ++ (is_error(query) ? " is-invalid" : "")}
      rows=10
      value=query_text
      onChange
    />
    <div className="invalid-feedback">
      {"Invalid query: " ++ get_error(query) |> React.string}
    </div>
    <button
      type_="button"
      className="btn btn-primary mt-3"
      disabled={Option.is_none(query) || is_error(query)}
      onClick>
      {"Execute" |> React.string}
    </button>
  </>;
};
