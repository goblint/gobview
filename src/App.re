open Goblint_lib;
open Batteries;
open Js_of_ocaml;
open Lwt.Infix;
module Cabs2cil = GoblintCil.Cabs2cil;

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
let renumber_goblint_analyses = registered_name => {
  let old_registered = Hashtbl.copy(MCP.registered);
  let old_registered_name = Hashtbl.copy(MCP.registered_name);
  Hashtbl.clear(MCP.registered);
  Hashtbl.clear(MCP.registered_name);
  Hashtbl.iter((name, id) => {
    let old_id = Hashtbl.find(old_registered_name, name);
    let spec = Hashtbl.find(old_registered, old_id);
    Hashtbl.replace(MCP.registered, id, spec);
    Hashtbl.replace(MCP.registered_name, name, id);
  }, registered_name);
};

let init_goblint = (solver, spec, registered_name, config, cil) => {
  AfterConfig.run(); // This registers the "base" analysis

  try(renumber_goblint_analyses(registered_name)) {
  | Not_found =>
    raise(InitFailed("Failed to renumber Goblint analyses"))
  };

  Sys.chdir("/"); // Don't remove this

  Sys_js.create_file(~name="/goblint/solver.marshalled", ~content=solver);
  Sys_js.create_file(~name="/goblint/config.json", ~content=config);
  Sys_js.create_file(~name="/goblint/spec_marshal", ~content=spec);

  GobConfig.merge_file(Fpath.v("/goblint/config.json"));

  GobConfig.set_string("dbg.level", "debug");
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

  // NOTE: Commenting this out since it breaks the node view. Semantic search
  // may depend on this code but it is currently broken because of unrelated
  // (and uknown) reasons anyway.
  // Cil.iterGlobals(cil, glob =>
  //   switch (glob) {
  //   | GFun(fd, _) =>
  //     Cil.prepareCFG(fd);
  //     Cil.computeCFGInfo(fd, true);
  //   | _ => ()
  //   }
  // );
  Cilfacade.current_file := cil;

  let goblint = GvGoblint.unmarshal(spec, cil);

  (goblint, cil);
};

let init = (solver, spec, config, meta, cil, analyses, warnings, stats, file_loc) => {
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

  let file_loc =
    switch (file_loc) {
    | Ok(s) => Marshal.from_string(s, 0)
    | _ => raise(InitFailed("Failed to load file path table"))
    };

  print_endline("Rendering app...");
  React.Dom.renderToElementWithId(
    <Main cil goblint warnings meta stats file_loc/>,
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
  "./solver.marshalled",
  "./spec_marshal",
  "./config.json",
  "./meta.json",
  "./cil.marshalled",
  "./analyses.marshalled",
  "./warnings.marshalled",
  "./stats.marshalled",
  "./file_loc.marshalled",
]
|> List.map(HttpClient.get)
|> Lwt.all
>>= (
  l =>
    Lwt.return(
      switch (l) {
      | [solver, spec, config, meta, cil, analyses, warnings, stats, file_loc] =>
        try(init(solver, spec, config, meta, cil, analyses, warnings, stats, file_loc)) {
        | exc => handle_error(exc)
        }
      | _ => ()
      },
    )
);
