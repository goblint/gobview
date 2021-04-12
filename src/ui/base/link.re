open Batteries;

[@react.component]
let make = (~url=?, ~class_=?, ~on_click=?, ~callback_data=?, ~children) => {
  let (url, class_, on_click, callback_data) =
    Utils.fix_opt_args4(url, class_, on_click, callback_data);
  let class_ = Option.default([], class_);

  let href =
    switch (url) {
    | Some(s) => s
    | _ => "#"
    };

  let className = String.concat(" ", class_);

  let onClick =
    switch (on_click, url) {
    | (None, None) => Some(ev => React.Event.Mouse.preventDefault(ev))
    | (None, Some(_)) => None
    | (Some(f), None) =>
      Some(
        ev => {
          React.Event.Mouse.preventDefault(ev);
          f(callback_data, ev);
        },
      )
    | (Some(f), Some(_)) => Some(f(callback_data))
    };

  <a href className ?onClick> children </a>;
};
