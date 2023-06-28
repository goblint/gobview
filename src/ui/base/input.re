[@react.component]
let make = (~type_=?, ~class_=?, ~value, ~on_change, ~on_submit=?, ~style=?, ~id=?) => {
  let (type_, class_, on_submit) =
    Utils.fix_opt_args3(type_, class_, on_submit);
  let type_ = Option.value(type_, ~default=`Text);
  let class_ = Option.value(class_, ~default=["form-control"]);
  let style_ = Option.value(style, ~default=React.Dom.Style.make());
  let id_ = Option.value(id, ~default="");
  
  let type_ =
    switch (type_) {
    | `Text => "text"
    };

  let className = String.concat(" ", class_);

  let onChange = ev => {
    React.Event.Synthetic.preventDefault(ev);
    React.Event.Synthetic.target(ev)
    |> Ojs.get_prop_ascii(_, "value")
    |> Ojs.string_of_js
    |> on_change;
  };

  let onKeyUp = ev =>
    if (React.Event.Keyboard.key(ev) == "Enter") {
      Option.iter(cb => cb(), on_submit);
    };

  switch id_ {
    | "" => <input type_ className value onChange onKeyUp style=style_ />;
    | _ => <input type_ className value onChange onKeyUp style=style_ id=id_ />;
  }

};
