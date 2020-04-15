let parsed_data = Parse.xml_data |> Parse.parse;

[@react.component]
let make = () => {
  let (line, setLine) = React.useState(() => -1);
  let (file, setFile) = React.useState(() => "test");
  
  <div style={ReactDOM.Style.make(~position="relative",())}>
    <div className="sidebar">
      <h2 className="title"> {"Gobview" |> React.string} </h2>
        <TreeList line calls={parsed_data |> Parse.get_calls}/>
    </div>
    <div className="sidebar globsidebar">
      <h2 className="title"> {"Globals" |> React.string} </h2>
        <GlobView globs={ parsed_data |> Parse.get_globs}/>
    </div>
    <div className="content-wrapper">
      <div className="content" style={ReactDOM.Style.make(~marginTop="30px",())}
        /* onClick={_ => {setLine(l => l+1)}} */
      >
        <h3>{file |> React.string}</h3>
        <CodeView dispatch=setLine calls={parsed_data |> Parse.get_calls} />
        <FileList files={parsed_data |> Parse.get_files} setFile />
        <p>{Parse.Test.zarith_string |> React.string}</p>
      </div>
    </div>
  </div>;
};
