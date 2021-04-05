open Batteries;

module ToggledSet = Set.Make(Int);

[@react.component]
let make = (~class_=?, ~style=?, ~override_class=?, ~children) => {
  let (class_, style, override_class) =
    Utils.fix_opt_args3(class_, style, override_class);
  let class_ = Option.default([], class_);
  let style = Option.default(`Default, style);

  let (toggled, set_toggled) = React.useState(() => ToggledSet.empty);

  let on_toggle = (i, ()) => {
    set_toggled(t =>
      if (ToggledSet.mem(i, t)) {
        ToggledSet.remove(i, t);
      } else {
        ToggledSet.add(i, t);
      }
    );
  };

  let get_class_name = (c, s) => {
    let style =
      switch (s) {
      | `Default => []
      | `Flush => ["list-group-flush"]
      };
    ();
    List.concat([["list-group"], c, style]) |> String.concat(" ");
  };

  let className =
    switch (override_class) {
    | Some(l) => String.concat(" ", l)
    | _ => get_class_name(class_, style)
    };

  <ul className>
    {React.Children.mapWithIndex(children, (elt, i) => {
       React.cloneElement(
         elt,
         CollapsibleListItem.makeProps(
           ~key=string_of_int(i),
           ~collapsed=!ToggledSet.mem(i, toggled),
           ~on_toggle=on_toggle(i),
           (),
         ),
       )
     })}
  </ul>;
};
