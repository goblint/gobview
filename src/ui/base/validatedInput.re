[@react.component]
let make = (~class_=?, ~value, ~on_change, ~children) => {
  let class_ = Option.value(~default=["form-control"], class_);

  let input_class =
    if (Result.is_error(value)) {
      ["is-invalid", ...class_];
    } else {
      class_;
    };

  let value = Result.fold(~ok=s => s, ~error=s => s, value);

  <>
    <Input class_=input_class value on_change />
    <InvalidFeedback> ...children </InvalidFeedback>
  </>;
};
