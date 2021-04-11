open Batteries;
open Js_of_ocaml;
open Lwt.Infix;

exception InitFailed(string);

let init_cil = environment => {
  // Restore the environment hash table, which
  // syntacticsearch uses to map temporary variables created
  // by CIL to their original counterparts.
  Hashtbl.clear(Cabs2cil.environment);
  environment
  |> Hashtbl.enum
  |> Enum.iter(((k, v)) => Hashtbl.add(Cabs2cil.environment, k, v));
};

// Each Goblint analysis module is assigned an ID, and this
// ID depends on the module registration order, which might
// differ from build to build. This function reorders the
// analysis list to match the native version of Goblint.
let reorder_goblint_analysis_list =
  List.iter(((id, ana)) => {
    let (current_id, _) =
      List.find(snd %> String.equal(ana), MCP.analyses_table^);
    let spec = List.assoc(current_id, MCP.analyses_list'^);
    let deps = List.assoc(current_id, MCP.dep_list'^);

    let actual_ana_with_id = List.assoc(id, MCP.analyses_table^);
    MCP.analyses_table :=
      MCP.analyses_table^
      |> List.map(((i, a)) =>
           if (i == id) {
             (i, ana);
           } else if (i == current_id) {
             (i, actual_ana_with_id);
           } else {
             (i, a);
           }
         );

    let actual_spec_with_id = List.assoc(id, MCP.analyses_list'^);
    MCP.analyses_list' :=
      MCP.analyses_list'^
      |> List.map(((i, s)) =>
           if (i == id) {
             (i, spec);
           } else if (i == current_id) {
             (i, actual_spec_with_id);
           } else {
             (i, s);
           }
         );

    let actual_deps_with_id = List.assoc(id, MCP.dep_list'^);
    MCP.dep_list' :=
      MCP.dep_list'^
      |> List.map(((i, d)) =>
           if (i == id) {
             (i, deps);
           } else if (i == current_id) {
             (i, actual_deps_with_id);
           } else {
             (i, d);
           }
         );
  });

let init_goblint = (solver, table, config, cil) => {
  reorder_goblint_analysis_list(table);
  try(reorder_goblint_analysis_list(table)) {
  | Not_found =>
    raise(InitFailed("Failed to populate the Goblint analysis list"))
  };

  Sys.chdir("/"); // Don't remove this

  Sys_js.create_file(~name="/goblint/solver.marshalled", ~content=solver);
  Sys_js.create_file(~name="/goblint/config.json", ~content=config);

  GobConfig.merge_file("/goblint/config.json");

  GobConfig.set_bool("dbg.verbose", true);
  // TODO: Uncomment this to improve performance in future
  // GobConfig.set_bool("verify", false);

  GobConfig.set_string("load_run", "goblint");
  GobConfig.set_string("save_run", ""); // This will be set by config.json. Reset it

  GobConfig.set_auto("trans.activated[+]", "'expeval'");

  Cilfacade.init();
  Maingoblint.handle_extraspecials();
  Maingoblint.handle_flags();

  // Don't remove these either
  let cil = Cilfacade.callConstructors(cil);
  Cilfacade.createCFG(cil);
  Cilfacade.ugglyImperativeHack := cil;

  let goblint = GvGoblint.unmarshal(~goblint=solver, cil);

  (goblint, cil);
};

let init = (~pdata, ~solver, ~config, ~meta, ~cil, ~analyses, ~warnings, ()) => {
  let cil =
    switch (cil) {
    | Ok(s) =>
      let (c, e) = Marshal.from_string(s, 0);
      init_cil(e);
      c;
    | _ => raise(InitFailed("Failed to load CIL state"))
    };
  print_endline("Restored Cabs2cil.environment");

  let (goblint, cil) =
    switch (solver, analyses, config) {
    | (Ok(s), Ok(t), Ok(c)) =>
      let t = Marshal.from_string(t, 0);
      init_goblint(s, t, c, cil);
    | _ => raise(InitFailed("Failed to load Goblint state"))
    };
  print_endline("Initialized Goblint");

  let pdata =
    switch (pdata) {
    | Ok(s) => Parse.parse_string(s)
    | _ => raise(InitFailed("Failed to load the analysis results"))
    };
  print_endline("Fetched the analysis results");

  let meta =
    switch (meta) {
    | Ok(s) => Yojson.Safe.from_string(s)
    | _ => raise(InitFailed("Failed to load the warning table"))
    };

  let warnings =
    switch (warnings) {
    | Ok(s) => Marshal.from_string(s, 0)
    | _ => raise(InitFailed("Failed to load the warning table"))
    };
  print_endline("Restored the warning table");

  print_endline("Rendering app...");
  React.Dom.renderToElementWithId(
    <Main pdata cil goblint meta warnings />,
    "app",
  );
};

let handle_error = exc => {
  let s =
    switch (exc) {
    | InitFailed(s) => s
    | _ => Printexc.to_string(exc)
    };
  React.Dom.renderToElementWithId(s |> React.string, "app");
};

[
  "/analysis.xml",
  "/goblint/solver.marshalled",
  "/goblint/config.json",
  "/goblint/meta.json",
  "/goblint/cil.marshalled",
  "/goblint/analyses.marshalled",
  "/goblint/warnings.marshalled",
]
|> List.map(HttpClient.get)
|> Lwt.all
>>= (
  l =>
    Lwt.return(
      switch (l) {
      | [pdata, solver, config, meta, cil, analyses, warnings] =>
        try(
          init(
            ~pdata,
            ~solver,
            ~config,
            ~meta,
            ~cil,
            ~analyses,
            ~warnings,
            (),
          )
        ) {
        | exc => handle_error(exc)
        }
      | _ => ()
      },
    )
);
