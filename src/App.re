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

let init_goblint = (solver, spec, table, config, cil) => {
  AfterConfig.run(); // This registers the "base" analysis

  try(reorder_goblint_analysis_list(table)) {
  | Not_found =>
    raise(InitFailed("Failed to populate the Goblint analysis list"))
  };

  Sys.chdir("/"); // Don't remove this

  Sys_js.create_file(~name="/goblint/solver.marshalled", ~content=solver);
  Sys_js.create_file(~name="/goblint/config.json", ~content=config);
  Sys_js.create_file(~name="/goblint/spec_marshal", ~content=spec);

  GobConfig.merge_file("/goblint/config.json");

  GobConfig.set_bool("dbg.verbose", true);
  // TODO: Uncomment this to improve performance in future
  // GobConfig.set_bool("verify", false);

  // For some reason, the standard Batteries output channels
  // appear to be closed by default and writing to them
  // raises [BatInnerIO.Output_closed]. This fixes it.
  let out = IO.output_channel(Stdlib.stdout);
  Messages.formatter := Format.formatter_of_output(out);

  GobConfig.set_string("load_run", "goblint");

  // These two will be set by config.json. Reset them.
  GobConfig.set_string("save_run", "");
  GobConfig.set_bool("gobview", false);

  GobConfig.set_auto("trans.activated[+]", "'expeval'");

  Cilfacade.init();
  Maingoblint.handle_extraspecials();
  Maingoblint.handle_flags();

  // Don't remove these either
  Cil.iterGlobals(cil, glob =>
    switch (glob) {
    | GFun(fd, _) =>
      Cil.prepareCFG(fd);
      Cil.computeCFGInfo(fd, true);
    | _ => ()
    }
  );
  Cilfacade.current_file := cil;

  let goblint = GvGoblint.unmarshal(spec, cil);

  (goblint, cil);
};

let init = (solver, spec, config, meta, cil, analyses, warnings, stats) => {
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
    switch (solver, spec, analyses, config) {
    | (Ok(s), Ok(spec), Ok(t), Ok(c)) =>
      let t = Marshal.from_string(t, 0);
      init_goblint(s, spec, t, c, cil);
    | _ => raise(InitFailed("Failed to load Goblint state"))
    };
  print_endline("Initialized Goblint");

  let warnings =
    switch (warnings) {
    | Ok(s) => Marshal.from_string(s, 0)
    | _ => raise(InitFailed("Failed to load the warning table"))
    };
  print_endline("Restored the warning table");

  let meta =
    switch (meta) {
    | Ok(s) => Yojson.Safe.from_string(s)
    | _ => raise(InitFailed("Failed to load the warning table"))
    };

  let stats =
    switch (stats) {
    | Ok(s) => Marshal.from_string(s, 0)
    | _ => raise(InitFailed("Failed to load runtime stats"))
    };

  print_endline("Rendering app...");
  React.Dom.renderToElementWithId(
    <Main cil goblint warnings meta stats />,
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
  "/goblint/solver.marshalled",
  "/goblint/spec_marshal",
  "/goblint/config.json",
  "/goblint/meta.json",
  "/goblint/cil.marshalled",
  "/goblint/analyses.marshalled",
  "/goblint/warnings.marshalled",
  "/goblint/stats.marshalled",
]
|> List.map(HttpClient.get)
|> Lwt.all
>>= (
  l =>
    Lwt.return(
      switch (l) {
      | [solver, spec, config, meta, cil, analyses, warnings, stats] =>
        try(init(solver, spec, config, meta, cil, analyses, warnings, stats)) {
        | exc => handle_error(exc)
        }
      | _ => ()
      },
    )
);
