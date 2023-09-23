open Lwt.Infix;
open Init;

let init = (solver, spec, config, meta, cil, analyses, warnings, stats, file_loc) => {
  let cil =
    switch (cil) {
    | Ok(s) =>
      let (c, e) = Marshal.from_string(s, 0);
      Init.init_cil(e);
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
      }
    )
);
