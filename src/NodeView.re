[@react.component]
let make = () => {
  <Graphviz
    dot={
      "digraph cfg { 25 -> ret542 [label = \"return 0\"] ; }"
      |> Js_of_ocaml.Js.string
    }
  />;
};
