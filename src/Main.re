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
