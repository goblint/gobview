[@react.component]
let make = (~class_=?, ~value, ~on_change) => {
  let class_ = Utils.fix_opt_arg(class_);
  let class_ = Option.value(class_, ~default=["form-control"]);

  let className = String.concat(" ", class_);

  let onChange = ev => {
    React.Event.Synthetic.preventDefault(ev);
    React.Event.Synthetic.target(ev)
    |> Ojs.get(_, "value")
    |> Ojs.string_of_js
    |> on_change;
  };

  <textarea className value onChange />;
};
