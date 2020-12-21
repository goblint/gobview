type error =
  | Not_initialized_yet
  | Parse_error(string);

let string_of_error = e =>
  switch (e) {
  | Not_initialized_yet => "Not initialized yet"
  | Parse_error(e) => e
  };

let parse = s => {
  switch (Yojson.Safe.from_string(s)) {
  | json =>
    switch (CodeQuery.query_of_yojson(json)) {
    | Ppx_deriving_yojson_runtime.Result.Ok(q) => Ok(q)
    | Ppx_deriving_yojson_runtime.Result.Error(e) => Error(Parse_error(e))
    }
  | exception (Yojson.Json_error(e)) => Error(Parse_error(e))
  };
};

[@react.component]
let make = (~cil) => {
  let (query, set_query) = React.useState(() => "");
  let (parse_result, set_parse_result) =
    React.useState(() => Error(Not_initialized_yet));

  let onChange = ev => {
    let v =
      React.Event.Synthetic.target(ev)
      |> Ojs.get(_, "value")
      |> Ojs.string_of_js;
    set_query(_ => v);
    set_parse_result(_ => parse(v));
  };

  let onSubmit = ev => {
    React.Event.Form.preventDefault(ev);
    parse_result
    |> Result.iter(q =>
         QueryMapping.map_query(q, cil)
         |> ResultPrinter.print_result(_, q)
         |> Util.log
       );
  };

  let is_actual_error =
    Result.fold(
      ~error=
        e =>
          switch (e) {
          | Not_initialized_yet => false
          | _ => true
          },
      ~ok=_ => false,
      _,
    );

  let get_error_msg = Result.fold(~error=string_of_error, ~ok=_ => "", _);

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
              "form-control"
              ++ (is_actual_error(parse_result) ? " is-invalid" : "")
            }
            rows=10
            value=query
            onChange
          />
          <div className="invalid-feedback">
            {"Invalid query: " ++ get_error_msg(parse_result) |> React.string}
          </div>
        </div>
        <button
          type_="submit"
          className="btn btn-primary"
          disabled={Result.is_error(parse_result)}>
          {"Execute" |> React.string}
        </button>
      </form>
    </div>
  </div>;
};
