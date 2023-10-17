open React.Dom.Dsl.Html;
open Batteries;

[@react.component]
let make = (~url=?, ~class_=?, ~on_click=?, ~children) => {
  let (url, class_, on_click) =
    Utils.fix_opt_args3(url, class_, on_click);
  let class_ = Option.default([], class_);

  let href =
    switch (url) {
    | Some(s) => s
    | _ => "#"
    };

  let className = String.concat(" ", class_);

  let onClick =
    switch (on_click, url) {
    | (None, None) => Some(ev => React.Event.Mouse.prevent_default(ev))
    | (None, Some(_)) => None
    | (Some(f), _) =>
      Some(
        ev => {
          React.Event.Mouse.prevent_default(ev);
          f(ev);
        },
      )
    };

  <a href className ?onClick> children </a>;
};
