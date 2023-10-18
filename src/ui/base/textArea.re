open React.Dom.Dsl.Html;

[@react.component]
let make = (~class_=?, ~value, ~on_change) => {
  let class_ = Option.value(class_, ~default=["form-control"]);

  let className = String.concat(" ", class_);

  let onChange = ev => {
    React.Event.Synthetic.prevent_default(ev);
    React.Event.Synthetic.target(ev)
    |> Ojs.get_prop_ascii(_, "value")
    |> Ojs.string_of_js
    |> on_change;
  };

  <textarea className value onChange />;
};
