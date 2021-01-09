[@react.component]
let make = (~on_submit, ~children) => {
  let onSubmit = ev => {
    React.Event.Form.preventDefault(ev);
    on_submit();
  };
  <form onSubmit> children </form>;
};
