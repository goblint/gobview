open React.Dom.Dsl.Html;
open Batteries;

module SelectedSidebar = State.SelectedSidebar;

let views = [
  (SelectedSidebar.Nodes, "Nodes"),
  (SelectedSidebar.Globals, "Globals"),
];

let make_nav = (active, dispatch) => {
  let on_click = (v, _) => dispatch @@ `SwitchSidebarRight(v);

  <ul className="nav nav-tabs">
    ...{views
     |> List.mapi((i, (v, n)) => {
          let class_ =
            [["nav-link"], v == active ? ["active"] : []] |> List.concat;
          <li key={string_of_int(i)} className="nav-item">
            <Link class_ on_click={on_click(v)}>
              ...{n |> React.string}
            </Link>
          </li>;
        })}
  </ul>;
};

[@react.component]
let make = (~active, ~goblint, ~inspect, ~dispatch) => {
  <div className="sidebar-right">
    {make_nav(active, dispatch)}
    <div className="tab-content">
      <div className="tab-pane active">
        {switch (active) {
         | Nodes => <GvNodeStateView goblint inspect />
         | Globals => <GvGlobalStateView analyses=goblint#global_analyses />
         | _ => <div/>
         }}
      </div>
    </div>
  </div>;
};
