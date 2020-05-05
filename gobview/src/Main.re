open Util;

[@react.component]
let make = () => {
  let (line, setLine) = React.useState(() => -1);
  let (file, setFile) = React.useState(() => "");
  let (filepath, setFilepath) = React.useState(() => "");
  let (pdata, setPdata) = React.useState(() => (Parse.empty_run));
  let (code, setCode) = React.useState(() => "");
  let (showNodeView, setShowNodeView) = React.useState(() => false);
  let fetchCode = (s) => {let _ = Lwt.bind(Datafetcher.http_get_with_base(s), (s => { setCode(_ => s); Lwt.return(())}));();}
  let fetchData = (s) => {
    let _ = Lwt.bind(Datafetcher.http_get_with_base(s), 
    (s => { 
      log("Parse data"); 
      let data = Parse.parse_string(s);
      setPdata(_ => data); 
      log("Parse data done"); 
      log("Search main");
      let (xfile, xfilepath) = Parse.search_main_file(Parse.get_files(data));
      if(String.equal(xfile, "")){
        log("Found no main file");
      }else {
        log("Found main file: " ++ xfile);
      }
      setFile(_ => xfile);
      setFilepath(_ => xfilepath);
      Lwt.return(())
    }));
  }

  React.useEffect0(() => {
    log("Initial data and code fetch");
    fetchData("analysis.xml");
    None;
  });

  React.useEffect1(() => {
    if(!String.equal(file, "")){
      fetchCode(file);
      log("Fetched code: " ++ file);
    }
    setLine(_=> -1);
    None;
  }, [|file|]);

  <div className="relative">
    <div className="sidebar">
      <h2 className="title"> {"Gobview" |> React.string} </h2>
        <CallView line calls={pdata |> Parse.get_calls} filepath/>
    </div>
    <div className="sidebar globsidebar">
      <h2 className="title"> {"Globals" |> React.string} </h2>
        <GlobView globs={ pdata |> Parse.get_globs}/>
    </div>
    <div className="content-wrapper">
      <div className="content">
        <div>
          <h3 style={ReactDOM.Style.make(~display="inline-block",())}>{file |> React.string}</h3>
          <button onClick={_ => setShowNodeView(x => !x)}
            style={ReactDOM.Style.make(~float="right",~marginTop="24px", ())}>
            { (showNodeView ? "Code View" : "Node View")  |> React.string}</button>
        </div>
        <div style={ReactDOM.Style.make(~overflow="auto",~height="85vh", ())}>
          { if (!showNodeView){
            <CodeView dispatch=setLine calls={pdata |> Parse.get_calls} code=code line filepath warnings={pdata |> Parse.get_warnings} />
            } else {
              <NodeView />
            }
          }
          <FileList files={pdata |> Parse.get_files} setFile setFilepath />
          /* <p>{Parse.Test.zarith_string |> React.string}</p> */
        </div>
      </div>
    </div>
  </div>;
};
