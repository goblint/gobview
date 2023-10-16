open Batteries;
open Js_of_ocaml;

[@react.component]
let make = (~func: GvDisplay.func, ~dispatch) => {
  let show_info = id => {
    dispatch @@ `InspectNode(id |> Js.to_string);
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

  switch (func.dot) {
  | Some(dot) =>

    let graphiz = Graphviz.make(
      Graphviz.makeProps(
        ~dot={dot |> Js.string},
        ~options={Js.Unsafe.obj([|
          ("height", Js.Unsafe.inject("100%")),
          ("width", Js.Unsafe.inject("100%")),
          ("zoom", Js.Unsafe.inject(true)),
        |])},
        ~className={"fun-cfg" |> Js.string}, ()))


    ErrorBoundary.make(ErrorBoundary.makeProps(
      ~message={
        "Cannot display the function graph. The generated DOT file is probably too large."
        |> Js.string
      },
      ~children=graphiz,
      ()))
  | _ => React.null
  };
};
