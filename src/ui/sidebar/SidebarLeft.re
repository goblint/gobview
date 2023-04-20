open Batteries;

module SelectedSidebar = State.SelectedSidebar;

let views = [
  (SelectedSidebar.Files, "Files"),
  (SelectedSidebar.Search, "Search"),
];

let make_tabs = (active, dispatch) => {
  let on_click = (act, _) => Option.may(dispatch, act);

  <ul className="nav nav-tabs">
    {views
     |> List.mapi((i, (v, n)) => {
          let class_ =
            [["nav-link"], v == active ? ["active"] : []] |> List.concat;
          <li key={string_of_int(i)} className="nav-item">
            <Link class_ on_click callback_data={`SwitchSidebarLeft(v)}>
              {n |> React.string}
            </Link>
          </li>;
        })
     |> React.list}
  </ul>;
};

[@react.component]
let make = (~active, ~dispatch, ~search, ~cil) => {
  <div className="sidebar-left">
    {make_tabs(active, dispatch)}
    <div className="tab-content">
      <div className="tab-pane active">
        {switch (active) {
         | Files => <GvFileList cil dispatch />
         | Search => <SearchView search dispatch />
         | _ => <div/>
         }}
      </div>
    </div>
  </div>;
};
