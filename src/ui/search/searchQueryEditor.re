open React.Dom.Dsl.Html;
open Batteries;

let string_of_error = Option.map_default(Search.Query.string_of_error, "");

[@react.component]
let make = (~json, ~query, ~dispatch) => {
  let (query, error) = query;

  let text_area_class = [
    "form-control",
    ...Option.is_some(error) ? ["is-invalid"] : [],
  ];

  let on_change = s => {
    dispatch @@ `ParseSearchQuery(s);
  };

  let on_click = () => dispatch @@ `StartPerformSearch;

  <>
    <div className="mb-3">
      <TextArea class_=text_area_class value=json on_change />
      <InvalidFeedback>
        ...{"Invalid query: " ++ string_of_error(error) |> React.string}
      </InvalidFeedback>
    </div>
    <Button
      type_=`Button
      on_click
      disabled={Option.is_none(query) || Option.is_some(error)}>
      ...{"Execute" |> React.string}
    </Button>
  </>;
};
