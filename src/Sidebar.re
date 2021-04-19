open Batteries;

module SelectedSidebar = State.SelectedSidebar;

let views = [
  (SelectedSidebar.Nodes, "Nodes"),
  (SelectedSidebar.Globals, "Globals"),
];

let make_nav = (active, dispatch) => {
  let on_click = (act, _) => Option.may(dispatch, act);

  <ul className="nav nav-tabs">
    {views
     |> List.mapi((i, (v, n)) => {
          let class_ =
            [["nav-link"], v == active ? ["active"] : []] |> List.concat;
          <li key={string_of_int(i)} className="nav-item">
            <Link class_ on_click callback_data={`SwitchSidebar(v)}>
              {n |> React.string}
            </Link>
          </li>;
        })
     |> React.list}
  </ul>;
};

[@react.component]
let make = (~state: State.t, ~dispatch) => {
  let current = state.selected_sidebar;
  <>
    {make_nav(current, dispatch)}
    <div className="tab-content">
      <div className="tab-pane active">
        {switch (current) {
         | SelectedSidebar.Nodes =>
           <GvNodeStateView
             goblint={state.goblint}
             display={state.display}
             file_path={state.file_path}
             line={state.line}
             id={state.id}
           />
         | SelectedSidebar.Globals =>
           <GvGlobalStateView analyses=(state.goblint)#global_analyses />
         }}
      </div>
    </div>
  </>;
};
