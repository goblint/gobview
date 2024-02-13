open React.Dom.Dsl.Html;
open Batteries;

let options = [(`Must, "Must"), (`May, "May")];

[@react.component]
let make = (~value, ~on_change) => {
  <div className="mb-3">
    <Label> ...{"Mode" |> React.string} </Label>
    <Select options value on_change />
  </div>;
};
