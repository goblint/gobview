open Batteries;

[@react.component]
let make =
    (~name=?, ~collapsed=?, ~override_class=?, ~on_toggle=?, ~children=?) => {
  let (name, collapsed, override_class, on_toggle, children) =
    Utils.fix_opt_args5(name, collapsed, override_class, on_toggle, children);
  let name = Option.default("", name);
  let collapsed = Option.default(true, collapsed);
  let children = Option.default(React.null, children);

  let className =
    switch (override_class) {
    | Some(l) => String.concat(" ", l)
    | _ => "list-group-item"
    };

  <li className>
    <div>
      {name |> React.string}
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
