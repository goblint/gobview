open Batteries;
open Js_of_ocaml;
open GoblintCil;

let map_to_graph = map =>
  Hashtbl.fold(
    (caller, (callee, _), carry) => {
      let id = fn =>
        "\"" ++ fn.svar.vdecl.file ++ "," ++ fn.svar.vname ++ "\"";
      let new_entry =
        id(callee)
        ++ "[label=\""
        ++ callee.svar.vname
        ++ "\"];"
        ++ id(caller)
        ++ "[label=\""
        ++ caller.svar.vname
        ++ "\"]"
        ++ ";"
        ++ id(callee)
        ++ " -> "
        ++ id(caller)
        ++ ";";
      // Avoid duplicates
      if (String.exists(carry, new_entry)) {
        carry;
      } else {
        carry ++ "\n" ++ new_entry;
      };
    },
    map,
    // Setup to enable clicking the nodes and styling.
    "digraph{\n node [id=\"\\N\",URL=\"javascript:show_info('\\N');\",style=filled,fillcolor=white];\n",
  )
  ++ "}";
[@react.component]
let make = (~graph, ~ctx: option(Representation.t), ~dispatch) => {
  let show_info = decl_string => {
    decl_string
    |> Js.to_string
    |> String.split(~by=",")
    |> (((path, name)) => dispatch @@ `DisplayFunc((name, path)));

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
  let build_ctx_graph = c => {
    let copy = Hashtbl.copy(graph);
    // Only include calls, where the context matches
    Hashtbl.filter_map_inplace(
      (_, (_, f_c) as a) =>
        if (f_c == c) {
          Some(a);
        } else {
          None;
        },
      copy,
    );
    map_to_graph(copy);
  };
  let listOfNames =
    switch (ctx) {
    | None => map_to_graph(graph)
    | Some(c) => build_ctx_graph(c)
    };

  <ErrorBoundary
    message={
      "Cannot display the function graph. The generated DOT file is probably too large."
      |> Js.string
    }>
    <Graphviz
      dot={listOfNames |> Js.string}
      options={Js.Unsafe.obj([|
        ("height", Js.Unsafe.inject("100%")),
        ("width", Js.Unsafe.inject("100%")),
        ("zoom", Js.Unsafe.inject(true)),
      |])}
      className={"fun-cfg" |> Js.string}
    />
  </ErrorBoundary>;
};
