open Batteries

let options = [
  (true, "True"),
  (false, "False"),
];

[@react.component]
let make = (~value, ~on_change) => {
  <div className="mb-3">
    <Label> {"Join ?" |> React.string} </Label>
    <Select options value on_change />
  </div>;
};
