open Lwt;

let query = {|{
"kind": ["var"],
"target": ["name", "x"],
"find": ["uses"],
"structure": ["none"],
"limitation": ["none"],
"expression": "x == 42",
"mode": ["May"]
}|};

let analysis_order = [
  (0, "threadflag"),
  (1, "threadid"),
  (3, "base"),
  (8, "mutex"),
  (10, "expRelation"),
  (17, "mallocWrapper"),
  (33, "escape"),
];

// TODO: Each analysis is assigned an ID, and this ID is not guaranteed
// to be stable across builds. This sync the JS build with the native
// build on my device, but a more universal solution must be implemented
// at some point.
let rec fix_goblint_analysis_order = queue => {
  switch (queue) {
  | [] => ()
  | [(id, ana), ...rest] =>
    let (actual_id, _) =
      List.find(((_, a)) => a == ana, MCP.analyses_table^);

    let actual_ana_with_id = List.assoc(id, MCP.analyses_table^);
    let analyses_table =
      MCP.analyses_table^
      |> List.map(((i, a)) =>
           if (i == id) {
             (i, ana);
           } else if (i == actual_id) {
             (i, actual_ana_with_id);
           } else {
             (i, a);
           }
         );
    MCP.analyses_table := analyses_table;

    let spec = List.assoc(actual_id, MCP.analyses_list'^);
    let actual_spec_with_id = List.assoc(id, MCP.analyses_list'^);
    let analyses_list =
      MCP.analyses_list'^
      |> List.map(((i, s)) =>
           if (i == id) {
             (i, spec);
           } else if (i == actual_id) {
             (i, actual_spec_with_id);
           } else {
             (i, s);
           }
         );
    MCP.analyses_list' := analyses_list;

    let deps = List.assoc(actual_id, MCP.dep_list'^);
    let actual_deps_with_id = List.assoc(id, MCP.dep_list'^);
    let dep_list =
      MCP.dep_list'^
      |> List.map(((i, d)) =>
           if (i == id) {
             (i, deps);
           } else if (i == actual_id) {
             (i, actual_deps_with_id);
           } else {
             (i, d);
           }
         );
    MCP.dep_list' := dep_list;

    fix_goblint_analysis_order(rest);
  };
};

HttpClient.get("/test")
>>= (
  s => {
    print_endline(string_of_int(Sys.int_size));
    print_endline(string_of_int(Int.max_int));
    let _ =
      try(Marshal.from_string(s, 0)) {
      | e =>
        let s = Printexc.to_string(e);
        Printf.printf("Marshal failed: %s\n", Printexc.to_string(e));
        failwith(s);
      };
    // print_endline(string_of_int(i));
    Lwt.return_unit;
  }
);

Lwt.all([
  HttpClient.get_opt("/run/solver.marshalled"),
  HttpClient.get_opt("/run/config.json"),
  HttpClient.get_opt("/run/meta.json"),
])
>>= (
  l =>
    switch (l) {
    | [Some(s), Some(c), Some(m)] =>
      Js_of_ocaml.Sys_js.create_file(
        ~name="/run/solver.marshalled",
        ~content=s,
      );
      Js_of_ocaml.Sys_js.create_file(~name="/run/config.json", ~content=c);
      Js_of_ocaml.Sys_js.create_file(~name="/run/meta.json", ~content=m);
      Js_of_ocaml.Sys_js.create_file(~name="/query.json", ~content=query);
      fix_goblint_analysis_order(analysis_order);
      React.Dom.renderToElementWithId(<Main />, "app");
      Lwt.return_unit;
    | _ => Lwt.return_unit
    }
);
