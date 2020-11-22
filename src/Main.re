open SelectedView;
open State;
open Util;

[@react.component]
let make = () => {
  let (state, dispatch) = React.useReducer(reducer, default);

  let fetchCode = s => {
    let _ =
      Lwt.bind(
        Datafetcher.http_get_with_base(s),
        s => {
          dispatch @@ Set_code(s);
          Lwt.return();
        },
      );
    ();
  };

  let fetchData = s => {
    let _ =
      Lwt.bind(
        Datafetcher.http_get_with_base(s),
        s => {
          log("Parse data");
          let data = Parse.parse_string(s);
          dispatch @@ Set_pdata(data);
          log("Parse data done");
          log("Search main");
          let (xfile, xfilepath) =
            Parse.search_main_file(Parse.get_files(data));
          if (String.equal(xfile, "")) {
            log("Found no main file");
          } else {
            log("Found main file: " ++ xfile);
          };
          dispatch @@ Inspect_file(xfile, xfilepath);
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
      | Some(Func(f)) =>
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
              Datafetcher.http_get_with_base(url),
              dot => {
                dispatch @@ Update_dot(dot);
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
      <div className="col-3 border-right">
        <h2> {"State" |> React.string} </h2>
        <StateView state calls={state.pdata |> Parse.get_calls} />
      </div>
      // <div className="col-3 order-last border-left">
      //   <h2> {"Globals" |> React.string} </h2>
      //   <GlobView globs={state.pdata |> Parse.get_globs} />
      // </div>
      <div className="col-9">
        <Menu state dispatch />
        <div
          style={React.Dom.Style.make(~overflow="auto", ~height="85vh", ())}>
          {switch (state.selected_view) {
           | Content => <Content state dispatch />
           | Warning =>
             <WarningView
               dispatch
               warnings={state.pdata |> Parse.get_warnings}
             />
           | File =>
             <FileList dispatch files={state.pdata |> Parse.get_files} />
           | Parameters =>
             <ParameterView parameters={state.pdata |> Parse.get_parameters} />
           | DeadCode =>
             <DeadCodeView
               dispatch
               calls={
                 state.pdata
                 |> Parse.get_calls
                 |> List.filter(Parse.has_dead_code)
                 |> Parse.sort_calls_by_line
               }
             />
           | Statistics =>
             <StatisticsView
               statistics={state.pdata |> Parse.get_statistics}
             />
           }}
        </div>
      </div>
    </div>
  </div>;
};
