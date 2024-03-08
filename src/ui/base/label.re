open React.Dom.Dsl.Html;
open Batteries;

[@react.component]
let make = (~class_=?, ~children) => {
  let class_ = Option.default(["form-label"], class_);

  let className = String.concat(" ", class_);

  <label className> children </label>;
};
