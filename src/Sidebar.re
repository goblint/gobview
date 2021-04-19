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
let make = (~active, ~goblint, ~inspect, ~dispatch) => {
  <>
    {make_nav(active, dispatch)}
    <div className="tab-content">
      <div className="tab-pane active">
        {switch (active) {
         | SelectedSidebar.Nodes => <GvNodeStateView goblint inspect />
         | Globals => <GvGlobalStateView analyses=goblint#global_analyses />
         }}
      </div>
    </div>
  </>;
};
