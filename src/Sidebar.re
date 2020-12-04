open State;

let views = [
  (Selected_sidebar.State, "State"),
  (Selected_sidebar.Globals, "Globals"),
];

let make_nav_tabs = (side_panel, dispatch) => {
  let on_click = (act, ev) => {
    React.Event.Mouse.preventDefault(ev);
    dispatch @@ act;
  };

  <ul className="nav nav-tabs">
    {views
     |> List.mapi((i, (v, n)) => {
          <li key={string_of_int(i)} className="nav-item">
            <a
              href="#"
              className={"nav-link" ++ (side_panel == v ? " active" : "")}
              onClick={on_click(Switch_sidebar(v))}>
              {n |> React.string}
            </a>
          </li>
        })
     |> React.list}
  </ul>;
};

[@react.component]
let make = (~state, ~dispatch) => {
  let current = selected_sidebar(state);
  <>
    {make_nav_tabs(current, dispatch)}
    <div className="tab-content">
      <div className="tab-pane active">
        {switch (current) {
         | Selected_sidebar.State =>
           <StateView state calls={state |> pdata |> Parse.get_calls} />
         | Selected_sidebar.Globals =>
           <GlobView globs={state |> pdata |> Parse.get_globs} />
         }}
      </div>
    </div>
  </>;
};
