open Batteries;


[@react.component]
let make =
    (~name=?, ~collapsed=?, ~override_class=?, ~on_toggle=?, ~children=?, ~make_title=?) => {
  let (name, collapsed, override_class, on_toggle, children, make_title) =
    Utils.fix_opt_args6(name, collapsed, override_class, on_toggle, children, make_title);
  let name = Option.default("", name);
  let collapsed = Option.default(true, collapsed);
  let children = Option.default(React.null, children);
  let make_title = Option.default((name) => name |> React.string, make_title);


  let className =
    switch (override_class) {
    | Some(l) => String.concat(" ", l)
    | _ => "list-group-item"
    };

  <li className>
    <div>
      {make_title(name)}
      <Button
        class_=["btn", "btn-sm", "dropdown-toggle"]
        color=`None
        on_click=?on_toggle>
        React.null
      </Button>
    </div>
    {if (collapsed) {React.null} else {children}}
  </li>;
};
