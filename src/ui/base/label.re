open React.Dom.Dsl.Html;
open Batteries;

[@react.component]
let make = (~class_=?, ~children) => {
  let class_ = Utils.fix_opt_arg(class_) |> Option.default(["form-label"]);

  let className = String.concat(" ", class_);

  <label className> children </label>;
};
