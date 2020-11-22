open SelectedView;
open State;

let views = [
  (Content, "Content View"),
  (Warning, "Warnings"),
  (DeadCode, "Dead Code"),
  (File, "Select File"),
  (Parameters, "Parameters"),
  (Statistics, "Statistics"),
];

[@react.component]
let make = (~state, ~dispatch) => {
  <ul className="nav nav-tabs">
    {views
     |> List.mapi((i, (v, n)) => {
          <li key={string_of_int(i)} className="nav-item">
            <a
              className={
                "nav-link " ++ (state.selected_view == v ? "active" : "")
              }
              onClick={_ => dispatch @@ Set_selected_view(v)}>
              {n |> React.string}
            </a>
          </li>
        })
     |> React.list}
  </ul>;
};
