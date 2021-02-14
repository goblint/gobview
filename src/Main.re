open Util;

[@react.component]
let make = (~pdata, ~cil) => {
  let (state, dispatch) =
    React.useReducer(Reducer.reducer, State.create(~pdata, ~cil, ()));

  let fetchCode = s => {
    let _ =
      Lwt.bind(
        HttpClient.get("/" ++ s),
        s => {
          dispatch @@ `Set_code(Result.get_ok(s));
          Lwt.return();
        },
      );
    ();
  };

  let fetchData = s => {
    let _ =
      Lwt.bind(
        HttpClient.get("/" ++ s),
        s => {
          log("Parse data");
          let data = Parse.parse_string(Result.get_ok(s));
          log("Parse data done");
          log("Search main");
          let (xfile, xfilepath) =
            Parse.search_main_file(Parse.get_files(data));
          if (String.equal(xfile, "")) {
            log("Found no main file");
          } else {
            log("Found main file: " ++ xfile);
          };
          dispatch @@ `InspectFile((xfile, xfilepath));
          Lwt.return();
        },
      );
    ();
  };

  React.useEffect0(() => {
    log("Initial data and code fetch");
    fetchData("analysis.xml");
    None;
  });

  React.useEffect1(
    () => {
      if (!String.equal(state.file_name, "")) {
        fetchCode(state.file_name);
        log("Fetched code: " ++ state.file_name);
      };
      None;
    },
    [|state.file_name|],
  );

  React.useEffect0(() => {
    log("Fetching CIL dump");
    let _ =
      Lwt.bind(
        HttpClient.get("/cilfile.dump"),
        _ => {
          log("Fetched CIL dump");
          log("Loaded " ++ cil.Cil.fileName);

          let query =
            {|{"select":[["name"],["location"],["type"],["id"]],"type":["var"],"target":["name","x"],"find":["uses"]}|}
            |> Yojson.Safe.from_string
            |> CodeQuery.query_of_yojson
            |> (
              r => {
                switch (r) {
                | Ppx_deriving_yojson_runtime.Result.Ok(q) => Ok(q)
                | Ppx_deriving_yojson_runtime.Result.Error(e) => Error(e)
                };
              }
            )
            |> Result.get_ok;
          let results =
            try(QueryMapping.map_query(query, cil)) {
            | e =>
              print_endline(Printexc.to_string(e));
              raise(e);
            };
          print_endline(ResultPrinter.print_result(results, query));
          flush(stdout);

          let query = {|{
"kind": ["var"],
"target": ["name", "x"],
"find": ["uses"],
"structure": ["none"],
"limitation": ["none"],
"expression": "x == 42",
"mode": ["May"]
}|};
          Js_of_ocaml.Sys_js.create_file(~name="/query.json", ~content=query);
          try(GobCli.do_analyze(Analyses.empty_increment_data(), cil)) {
          | e => log(Printexc.to_string(e))
          };
          log("Goblint analysis is complete!");
          try(GobCli.do_analyze(Analyses.empty_increment_data(), cil)) {
          | e => log(Printexc.to_string(e))
          };
          log("Second Goblint analysis is complete!");

          Lwt.return();
        },
      );
    None;
  });

  React.useEffect1(
    () => {
      switch (state.inspect) {
      | Some(Graph(f)) =>
        if (Option.is_none(f.dot)) {
          let pattern = Js_of_ocaml.Regexp.regexp("/");
          let url =
            "dot/"
            ++ Js_of_ocaml.Regexp.global_replace(
                 pattern,
                 f.file_path,
                 "%252F",
               )
            ++ "/"
            ++ f.name
            ++ ".dot";
          log("Fetching " ++ url);
          let _ =
            Lwt.bind(
              HttpClient.get("/" ++ url),
              dot => {
                dispatch @@ `UpdateDot(Result.get_ok(dot));
                Lwt.return();
              },
            );
          ();
        }
      | _ => ()
      };
      None;
    },
    [|state.inspect|],
  );

  <div className="container-fluid">
    <div className="row">
      <div className="col-3 border-right"> <Sidebar state dispatch /> </div>
      <div className="col-9 d-flex flex-column vh-100">
        <Content state dispatch />
        <Panel state dispatch />
      </div>
    </div>
  </div>;
};
