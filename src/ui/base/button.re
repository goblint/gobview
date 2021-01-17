[@react.component]
let make =
    (~type_=?, ~class_=?, ~color=?, ~on_click=?, ~disabled=?, ~children) => {
  let (type_, class_, color, on_click, disabled) =
    Utils.fix_opt_args5(type_, class_, color, on_click, disabled);
  let type_ = Option.value(type_, ~default=`Button);
  let class_ = Option.value(class_, ~default=["btn"]);
  let color = Option.value(color, ~default=`Primary);
  let disabled = Option.value(disabled, ~default=false);

  let type_ =
    switch (type_) {
    | `Button => "button"
    | `Submit => "submit"
    };

  let color_class =
    switch (color) {
    | `Primary => ["btn-primary"]
    | _ => []
    };

  let className = [class_, color_class] |> List.concat |> String.concat(" ");

  let onClick = _ => Option.iter(cb => cb(), on_click);

  <button type_ className onClick disabled> children </button>;
};
