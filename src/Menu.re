open SelectedView;

let views = [
  (Code, "Code View"),
  (Node, "Node View"),
  (Warning, "Warnings"),
  (DeadCode, "Dead Code"),
  (File, "Select File"),
  (Parameters, "Parameters"),
  (Statistics, "Statistics"),
];

[@react.component]
let make = (~selectedView, ~setSelectedView) => {
  <ul className="nav nav-tabs">
    {views
     |> List.mapi((i, (v, n)) => {
          <li key={string_of_int(i)} className="nav-item">
            <a
              className={"nav-link " ++ (selectedView == v ? "active" : "")}
              onClick={_ => setSelectedView(_ => v)}>
              {n |> React.string}
            </a>
          </li>
        })
     |> React.list}
  </ul>;
};
