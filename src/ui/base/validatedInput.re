// jsoo-react seems to have some trouble with
// non-mandatory component properties. This is a
// workaround until the issue is resolved.
let make_extras = (~class_=["form-control"], ()) => {
  class_;
};

[@react.component]
let make = (~value, ~on_change, ~extras, ~children) => {
  let class_ = extras;

  let input_class =
    if (Result.is_error(value)) {
      ["is-invalid", ...class_];
    } else {
      class_;
    };

  let value = Result.fold(~ok=s => s, ~error=s => s, value);

  let extras = Input.make_extras(~class_=input_class, ());

  <>
    <Input value on_change extras />
    <InvalidFeedback> children </InvalidFeedback>
  </>;
};
