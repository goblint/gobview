[@react.component]
let make = () => {
  <Graphviz
    dot={
      {|digraph cfg {
        44 -> ret545 [label = "return
"] ;
        43 -> 44 [label = "assert(a[41] == 0)
"] ;
        42 -> 43 [label = "assert(a[7] == 0)
"] ;
        41 -> 42 [label = "assert(a[0] == 0)
"] ;
        39 -> 41 [label = ""] ;
        31 -> 41 [label = "Neg(i &lt; 42)
"] ;
        31 -> 41 [label = "Neg(1)
"] ;
        38 -> 31 [label = "i = i + 1
"] ;
        30 -> 31 [label = ""] ;
        27 -> 31 [label = "i = 0
"] ;
        37 -> 38 [label = "assert(a[17] == 0)
"] ;
        36 -> 37 [label = "assert(a[0] == 0)
"] ;
        35 -> 36 [label = "assert(a[i] == 0)
"] ;
        34 -> 35 [label = "a[i] = 0
"] ;
        31 -> 34 [label = "Pos(i &lt; 42)
"] ;
        fun545 -> 27 [label = "(body)
"] ;
        38 [id="38",URL="javascript:show_info('\N');",fillcolor=white,style=filled,];
        31 [id="31",URL="javascript:show_info('\N');",fillcolor=white,style=filled,shape=diamond];
        39 [id="39",URL="javascript:show_info('\N');",fillcolor=orange,style=filled,];
        ret545 [id="ret545",URL="javascript:show_info('\N');",fillcolor=white,style=filled,label ="return of example1()",shape=box];
        41 [id="41",URL="javascript:show_info('\N');",fillcolor=white,style=filled,];
        34 [id="34",URL="javascript:show_info('\N');",fillcolor=white,style=filled,];
        42 [id="42",URL="javascript:show_info('\N');",fillcolor=white,style=filled,];
        27 [id="27",URL="javascript:show_info('\N');",fillcolor=white,style=filled,];
        35 [id="35",URL="javascript:show_info('\N');",fillcolor=white,style=filled,];
        43 [id="43",URL="javascript:show_info('\N');",fillcolor=white,style=filled,];
        fun545 [id="fun545",URL="javascript:show_info('\N');",fillcolor=white,style=filled,label ="example1()",shape=box];
        36 [id="36",URL="javascript:show_info('\N');",fillcolor=white,style=filled,];
        44 [id="44",URL="javascript:show_info('\N');",fillcolor=white,style=filled,];
        37 [id="37",URL="javascript:show_info('\N');",fillcolor=white,style=filled,];
        30 [id="30",URL="javascript:show_info('\N');",fillcolor=orange,style=filled,];
}|}
      |> Js_of_ocaml.Js.string
    }
  />;
};
