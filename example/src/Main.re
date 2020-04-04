[@react.component]
let make = () => {
  /* let url = ReactRouter.useUrl(); */

  <div className="flex-container">
    <div className="sidebar">
      <h2 className="title"> {"Gobview" |> React.string} </h2>
        <TreeList />
    </div>
    <div className="content-wrapper">
      <div className="content" style={ReactDOM.Style.make(~marginTop="200px",())}>
        <CodeView />
      </div>
    </div>
  </div>;
};
