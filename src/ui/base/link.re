open Batteries;

[@react.component]
let make = (~url=?, ~class_=?, ~on_click=?, ~children) => {
  let (url, class_, on_click) = Utils.fix_opt_args3(url, class_, on_click);
  let class_ = Option.default([], class_);

  let href =
    switch (url) {
    | Some(s) => s
    | _ => "#"
    };

  let className = String.concat(" ", class_);

  let onClick =
    on_click
    |> Option.map((cb, ev) => {
         React.Event.Mouse.preventDefault(ev);
         cb();
       });

  <a href className ?onClick> children </a>;
};
