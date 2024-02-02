open React.Dom.Dsl.Html;
open Batteries;

[@react.component]
let make = (~value, ~on_change) => {
  <div className="mb-3">
    <Label> ...{"Expression" |> React.string} </Label>
    <Input value on_change />
  </div>;
};
