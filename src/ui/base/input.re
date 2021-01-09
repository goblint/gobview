// jsoo-react seems to have some trouble with
// non-mandatory component properties. This is a
// workaround until the issue is resolved.
let make_extras = (~type_="text", ()) => {
  type_;
};

[@react.component]
let make = (~value, ~on_change, ~extras) => {
  let type_ = extras;

  let onChange = ev => {
    React.Event.Synthetic.preventDefault(ev);
    React.Event.Synthetic.target(ev)
    |> Ojs.get(_, "value")
    |> Ojs.string_of_js
    |> on_change;
  };

  <input type_ className="form-control" value onChange />;
};
