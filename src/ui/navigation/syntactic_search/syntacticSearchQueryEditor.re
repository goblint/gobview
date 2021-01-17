module S = State.SyntacticSearch;

let string_of_error = e =>
  Option.map(S.Query.string_of_error, e) |> Option.value(~default="");

[@react.component]
let make = (~query_json, ~query, ~query_json_error, ~dispatch) => {
  let on_submit = () => {
    dispatch @@ `PerformSynSearch;
  };

  let text_area_class = [
    "form-control",
    ...Option.is_some(query_json_error) ? ["is-invalid"] : [],
  ];

  let on_change = s => {
    dispatch @@ `ParseSynSearchQuery(s);
  };

  <Form on_submit>
    <div className="mb-3">
      <TextArea class_=text_area_class value=query_json on_change />
      <InvalidFeedback>
        {"Invalid query: "
         ++ string_of_error(query_json_error)
         |> React.string}
      </InvalidFeedback>
    </div>
    <Button type_=`Submit disabled={Option.is_none(query)}>
      {"Execute" |> React.string}
    </Button>
  </Form>;
};
