[@react.component]
let make = () => {
  let (line, setLine) = React.useState(() => -1);
  let (file, setFile) = React.useState(() => "code.c");
  let (pdata, setPdata) = React.useState(() => (Parse.xml_data |> Parse.parse));
  let (code, setCode) = React.useState(() => "");
  let (showsvg, setShowsvg) = React.useState(() => false);
  let fetchCode = (s) => {let _ = Lwt.bind(Datafetcher.http_get_with_base(s), (s => { setCode(_ => s); Lwt.return(())}));();}
  let fetchData = (s) => {
    let _ = Lwt.bind(Datafetcher.http_get_with_base(s), 
    (s => { 
      Datafetcher.log("Parse data"); 
      setPdata(_ => (Parse.parse_string(s))); 
      Datafetcher.log("Parse data done"); 
      Lwt.return(())
    }));
  }

  React.useEffect0(() => {
    Datafetcher.log("Initial data and code fetch");
    fetchData("data.xml");
    /* fetchCode("code.c"); */
    None;
  });

  React.useEffect1(() => {
    fetchCode(file);
    Datafetcher.log("fetch code");
    None;
  }, [|file|]);

  <div style={ReactDOM.Style.make(~position="relative",())}>
    <div className="sidebar">
      <h2 className="title"> {"Gobview" |> React.string} </h2>
        <TreeList line calls={pdata |> Parse.get_calls}/>
    </div>
    <div className="sidebar globsidebar">
      <h2 className="title"> {"Globals" |> React.string} </h2>
        <GlobView globs={ pdata |> Parse.get_globs}/>
    </div>
    <div className="content-wrapper">
      <div className="content">
        <div>
          <h3 style={ReactDOM.Style.make(~display="inline-block",())}>{file |> React.string}</h3>
          <button onClick={_ => setShowsvg(x => !x)}
            style={ReactDOM.Style.make(~float="right",~marginTop="24px", ())}>
            { (showsvg ? "Code View" : "Node View")  |> React.string}</button>
        </div>
        <div style={ReactDOM.Style.make(~overflow="auto",~height="85vh", ())}>
          {!showsvg ? <CodeView dispatch=setLine calls={pdata |> Parse.get_calls} code=code /> : 
            <img src={Datafetcher.base_url ++ "code2.svg"} width="100%"/>
          }
          <FileList files={pdata |> Parse.get_files} setFile />
          /* <p>{Parse.Test.zarith_string |> React.string}</p> */
        </div>
      </div>
    </div>
  </div>;
};
