open React.Dom.Dsl.Html;
open Batteries;

module ToggledSet = Set.Make(Int);

[@react.component]
let make = (~collapsed=?, ~class_=?, ~style=?, ~override_class=?, ~children) => {
  let collapsed = Option.default(true, collapsed);
  let class_ = Option.default([], class_);
  let style = Option.default(`Default, style);

  let (toggled, set_toggled) = React.use_state(() => ToggledSet.empty);

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
    {React.Children.mapi(children, (elt, i) => {
       React.clone_element(
         elt,
         CollapsibleListItem.make(
           ~key=string_of_int(i),
           ~collapsed={
             ToggledSet.mem(i, toggled) != collapsed;
           },
           ~on_toggle=on_toggle(i),
           (),
         ),
       )
     })}
  </ul>;
};
