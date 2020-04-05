[@react.component]
let make = () => {
  let (line, setLine) = React.useState(() => -1);
  
  <div className="flex-container">
    <div className="sidebar">
      <h2 className="title"> {"Gobview" |> React.string} </h2>
        <TreeList line/>
    </div>
    <div className="content-wrapper">
      <div className="content" style={ReactDOM.Style.make(~marginTop="30px",())}
        /* onClick={_ => {setLine(l => l+1)}} */
      >
        <CodeView dispatch=setLine />
      </div>
    </div>
  </div>;
};
