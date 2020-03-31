let s = React.string;

type example = {
  path: string,
  title: string,
  element: React.element,
  code: React.element,
};

let firstExample = {
  path: "hello-world-ocaml",
  title: "Hello World OCaml",
  element: <HelloWorldOCaml />,
  code: <Code text=[%blob "HelloWorldOCaml.ml"] />,
};
let examples = [
  firstExample,
  {
    path: "hello-world-reason",
    title: "Hello World Reason",
    element: <HelloWorldReason />,
    code: <Code text=[%blob "HelloWorldReason.re"] />,
  },
  {
    path: "events",
    title: "Events",
    element: <Events />,
    code: <Code text=[%blob "Events.re"] />,
  },
  {
    path: "greeting",
    title: "GreetingReason",
    element: <GreetingReason />,
    code: <Code text=[%blob "GreetingReason.re"] />,
  },
  {
    path: "gobview",
    title: "Gobview",
    element: <Gobview />,
    code: <Code text=[%blob "Refs.re"] />,
  }
];

[@react.component]
let make = () => {
  let url = ReactRouter.useUrl();

  <div className="flex-container">
    <div className="sidebar">
      <h2 className="title"> {"jsoo-react" |> s} </h2>
      <nav className="menu">
        <ul>
          {examples
           |> List.map(e => {
                <li key={e.path}>
                  <a
                    href={e.path}
                    onClick={event => {
                      ReactEvent.Mouse.preventDefault(event);
                      ReactRouter.push(e.path);
                    }}>
                    {e.title |> s}
                  </a>
                </li>
              })
           |> React.list}
        </ul>
      </nav>
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
           <h2> {example.title |> s} </h2>
           <h4> {"Rendered component" |> s} </h4>
           {example.element}
           <h4> {"Code" |> s} </h4>
           {example.code}
         </div>}
      </div>
    </div>
  </div>;
};
