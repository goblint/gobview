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

  let onSubmit = ev => {
    React.Event.Form.preventDefault(ev);
    dispatch @@ S.Execute_query;
  };

  let is_error = q =>
    Option.map(Result.is_error, q) |> Option.value(~default=false);

  let get_error = q =>
    switch (q) {
    | Some(Error(e)) => SS.string_of_error(e)
    | _ => ""
    };

  <div className="card">
    <div className="card-body">
      <h5 className="card-title"> {"Syntactic Search" |> React.string} </h5>
      <form className="needs-validation" noValidate=true onSubmit>
        <div className="mb-3">
          <label htmlFor="syntacticSearchQueryInput" className="form-label">
            {"Enter a query:" |> React.string}
          </label>
          <textarea
            id="syntacticSearchQueryInput"
            className={
              "form-control" ++ (is_error(query) ? " is-invalid" : "")
            }
            rows=10
            value=query_text
            onChange
          />
          <div className="invalid-feedback">
            {"Invalid query: " ++ get_error(query) |> React.string}
          </div>
        </div>
        <button
          type_="submit"
          className="btn btn-primary"
          disabled={Option.is_none(query) || is_error(query)}>
          {"Execute" |> React.string}
        </button>
      </form>
    </div>
  </div>;
};
