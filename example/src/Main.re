let s = React.string;

type example = {
  path: string,
  title: string,
  element: React.element,
  code: React.element,
};

let firstExample = {
    path: "gobview",
    title: "Gobview",
    element: <Gobview />,
    code: <Code text=[%blob "Refs.re"] />,
  };

let examples = [firstExample];

[@react.component]
let make = () => {
  let url = ReactRouter.useUrl();

  <div className="flex-container">
    <div className="sidebar">
      <h2 className="title"> {"Gobview" |> s} </h2>
        <Gobview />
    </div>
    <div className="content-wrapper">
      <div className="content">
        {let example =
           examples
           |> List.find_opt(e => {
                e.path
                == (List.nth_opt(url.path, 0) |> Option.value(~default=""))
              })
           |> Option.value(~default=firstExample);
         <div>
           {example.code}
         </div>}
      </div>
    </div>
  </div>;
};
