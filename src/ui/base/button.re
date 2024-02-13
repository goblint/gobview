open React.Dom.Dsl.Html;
open Batteries;

[@react.component]
let make =
    (
      ~type_=?,
      ~class_=?,
      ~color=?,
      ~outline=?,
      ~on_click=?,
      ~disabled=?,
      ~children,
    ) => {
  let type_ = Option.default(`Button, type_);
  let class_ = Option.default(["btn"], class_);
  let color = Option.default(`Primary, color);
  let outline = Option.default(false, outline);
  let disabled = Option.default(false, disabled);

  let type_ =
    switch (type_) {
    | `Button => "button"
    | `Submit => "submit"
    };

  let color_class =
    (
      switch (color) {
      | `Primary => ["primary"]
      | `Danger => ["danger"]
      | `None => []
      }
    )
    |> List.map(
         if (outline) {
           (++)("outline-");
         } else {
           identity;
         },
       )
    |> List.map((++)("btn-"));

  let className = [class_, color_class] |> List.concat |> String.concat(" ");

  let onClick = _ => Option.apply(on_click, ());

  <button type_ className onClick disabled> children </button>;
};
