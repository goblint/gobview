open State;

let views = [
  (SelectedSidebar.State, "State"),
  (SelectedSidebar.OldGlobals, "Globals"),
  (SelectedSidebar.Nodes, "Nodes"),
  (SelectedSidebar.Globals, "Globals"),
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
              onClick={on_click(`SwitchSidebar(v))}>
              {n |> React.string}
            </a>
          </li>
        })
     |> React.list}
  </ul>;
};

[@react.component]
let make = (~state, ~dispatch) => {
  let current = state.selected_sidebar;
  <>
    {make_nav_tabs(current, dispatch)}
    <div className="tab-content">
      <div className="tab-pane active">
        {switch (current) {
         | SelectedSidebar.State =>
           <StateView state calls={state |> pdata |> Parse.get_calls} />
         | SelectedSidebar.OldGlobals =>
           <GlobView globs=(state.goblint)#globs />
         | SelectedSidebar.Nodes => <GvNodeStateView goblint={state.goblint} />
         | SelectedSidebar.Globals =>
           <GvGlobalStateView analyses=(state.goblint)#global_analyses />
         }}
      </div>
    </div>
  </>;
};
