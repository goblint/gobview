open Js_of_ocaml;
open Reducer;
open State;

[@react.component]
let make = (~state, ~dispatch) => {
  let show_info = id => {
    dispatch @@ Set_id(id);
    // When you click on a link like `javascript:show_info('42')` in Firefox, it
    // replaces the contents of the current page with the return value of
    // `show_info('42')`. Therefore, this function must explicitly return
    // `undefined`. DO NOT REMOVE THIS!
    Js.undefined;
  };

  /*
   * Don't remove the underscore at the end of `show_info_`.
   * Otherwise, the function is mapped as `show`.
   */
  Js.(Unsafe.global##.show_info_ := wrap_callback(show_info));

  let dot =
    switch (state.inspect) {
    | Some(Func(f)) => Inspect.Func.dot_opt(f)
    | _ => None
    };

  switch (dot) {
  | Some(dot) =>
    <ErrorBoundary
      message={
        "Cannot display graph. The generated DOT file is probably too large."
        |> Js.string
      }>
      <Graphviz dot={dot |> Js.string} />
    </ErrorBoundary>
  | _ => React.null
  };
};
