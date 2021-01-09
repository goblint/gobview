// jsoo-react seems to have some trouble with
// non-mandatory component properties. This is a
// workaround until the issue is resolved.
let make_optionals = (~on_submit=?, ()) => {
  on_submit;
};

[@react.component]
let make = (~optionals, ~children) => {
  let on_submit = optionals;
  let onSubmit = ev => {
    React.Event.Form.preventDefault(ev);
    Option.iter(cb => cb(ev), on_submit);
  };
  <form onSubmit> children </form>;
};
