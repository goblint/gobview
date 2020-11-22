open Js_of_ocaml;
open State;

[@react.component]
let make = (~state, ~dispatch) => {
  let show_info = a => dispatch @@ Set_id(a);

  /*
   * Don't remove the underscore at the end of `show_info_`.
   * Otherwise, the function is mapped as `show`.
   */
  Js.(Unsafe.global##.show_info_ := wrap_callback(show_info));

  <>
    <NodeViewFuncList dispatch pdata={state.pdata} />
    {switch (State.function_dot_opt(state)) {
     | Some(dot) => <Graphviz dot={dot |> Js_of_ocaml.Js.string} />
     | _ => React.null
     }}
  </>;
};
