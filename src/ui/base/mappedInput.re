// jsoo-react seems to have some trouble with
// non-mandatory component properties. This is a
// workaround until the issue is resolved.
let make_extras = (~label=?, ()) => {
  label;
};

[@react.component]
let make = (~value, ~from_string, ~to_string, ~on_change, ~extras, ~children) => {
  let label = extras;

  let onChange = ev => {
    React.Event.Synthetic.preventDefault(ev);
    React.Event.Synthetic.target(ev)
    |> Ojs.get(_, "value")
    |> Ojs.string_of_js
    |> from_string
    |> on_change;
  };

  let className =
    switch (value) {
    | Ok(_) => "form-control"
    | _ => "form-control is-invalid"
    };

  let value = to_string(value);

  <div>
    {switch (label) {
     | Some(s) => <label className="form-label"> {React.string(s)} </label>
     | _ => React.null
     }}
    <input type_="text" className value onChange />
    <InvalidFeedback> children </InvalidFeedback>
  </div>;
};
