open Util;
open SelectedView;

[@react.component]
let make = () => {
  let (id, setId) = React.useState(() => (-1));
  let (line, setLine) = React.useState(() => (-1));
  let (file, setFile) = React.useState(() => "");
  let (filepath, setFilepath) = React.useState(() => "");
  let (pdata, setPdata) = React.useState(() => Parse.empty_run);
  let (code, setCode) = React.useState(() => "");
  let (selectedView, setSelectedView) = React.useState(() => Code);

  let fetchCode = s => {
    let _ =
      Lwt.bind(
        Datafetcher.http_get_with_base(s),
        s => {
          setCode(_ => s);
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
          setPdata(_ => data);
          log("Parse data done");
          log("Search main");
          let (xfile, xfilepath) =
            Parse.search_main_file(Parse.get_files(data));
          if (String.equal(xfile, "")) {
            log("Found no main file");
          } else {
            log("Found main file: " ++ xfile);
          };
          setFile(_ => xfile);
          setFilepath(_ => xfilepath);
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
      if (!String.equal(file, "")) {
        fetchCode(file);
        log("Fetched code: " ++ file);
      };
      None;
    },
    [|file|],
  );

  <div className="container-fluid">
    <div className="row">
      <div className="relative">
        <div className="sidebar">
          <h2 className="title"> {"Gobview" |> React.string} </h2>
          <StateView
            selectedView
            id
            line
            calls={pdata |> Parse.get_calls}
            filepath
          />
        </div>
        <div className="sidebar globsidebar">
          <h2 className="title"> {"Globals" |> React.string} </h2>
          <GlobView globs={pdata |> Parse.get_globs} />
        </div>
        <div className="content-wrapper">
          <div className="content">
            <Menu selectedView setSelectedView />
            {if (selectedView == Code) {
               <div>
                 <h3 className="filename"> {file |> React.string} </h3>
               </div>;
             } else {
               React.null;
             }}
            <div
              style={React.Dom.Style.make(
                ~overflow="auto",
                ~height="85vh",
                (),
              )}>
              {switch (selectedView) {
               | Code =>
                 <CodeView
                   dispatch=setLine
                   calls={pdata |> Parse.get_calls}
                   code
                   line
                   filepath
                   warnings={pdata |> Parse.get_warnings}
                 />
               | Node => <NodeView dispatch=setId />
               | Warning =>
                 <WarningView
                   setFile
                   setFilepath
                   setSelectedView
                   setLine
                   warnings={pdata |> Parse.get_warnings}
                 />
               | File =>
                 <FileList
                   files={pdata |> Parse.get_files}
                   setFile
                   setFilepath
                   setSelectedView
                 />
               | Parameters =>
                 <ParameterView parameters={pdata |> Parse.get_parameters} />
               | DeadCode =>
                 <DeadCodeView
                   setFile
                   setFilepath
                   setSelectedView
                   setLine
                   calls={
                     pdata
                     |> Parse.get_calls
                     |> List.filter(Parse.has_dead_code)
                     |> Parse.sort_calls_by_line
                   }
                 />
               | Statistics =>
                 <StatisticsView statistics={pdata |> Parse.get_statistics} />
               }}
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>;
};
