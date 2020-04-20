[@react.component]
let make = () => {
  let (line, setLine) = React.useState(() => -1);
  let (file, setFile) = React.useState(() => "test");
  let (pdata, setPdata) = React.useState(() => (Parse.xml_data |> Parse.parse));
  let (code, setCode) = React.useState(() => (CodeData.c_code));
  React.useEffect0(() => {
    let _ = Lwt.bind(Datafetcher.http_get_with_base("data.xml"), (s => { setPdata(_ => (Parse.parse_string(s))); Lwt.return(())}));
    let _ = Lwt.bind(Datafetcher.http_get_with_base("code.c"), (s => { setCode(_ => s); Lwt.return(())}));
    Datafetcher.log("fetch data"); None;
  });

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
      <div className="content" style={ReactDOM.Style.make(~marginTop="30px",())}
        /* onClick={_ => {setLine(l => l+1)}} */
      >
        <h3>{file |> React.string}</h3>
        <CodeView dispatch=setLine calls={pdata |> Parse.get_calls} code=code />
        <FileList files={pdata |> Parse.get_files} setFile />
        /* <p>{Parse.Test.zarith_string |> React.string}</p> */
      </div>
    </div>
  </div>;
};
