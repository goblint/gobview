open Batteries;
open State;

let views = [
  (SelectedPanel.Warnings, "Warnings"),
  (SelectedPanel.DeadCode, "Dead code"),
  (SelectedPanel.Parameters, "Parameters"),
  (SelectedPanel.Statistics, "Statistics"),
];

let make_nav_pills = (current, dispatch) => {
  let on_click = (act, ev) => {
    React.Event.Mouse.preventDefault(ev);
    dispatch @@ act;
  };

  <ul className="nav nav-pills border-top border-bottom">
    {views
     |> List.mapi((i, (v, n)) => {
          <li key={string_of_int(i)} className="nav-item">
            <a
              href="#"
              className={"nav-link" ++ (current == Some(v) ? " active" : "")}
              onClick={on_click(`SwitchPanel(Some(v)))}>
              {n |> React.string}
            </a>
          </li>
        })
     |> React.list}
  </ul>;
};

[@react.component]
let make = (~state, ~dispatch) => {
  let parameters =
    switch (Yojson.Safe.Util.member("command", state.meta)) {
    | `String(command) => command
    | _ => ""
    };

  let locations = (state.goblint)#dead_locations;

  let current = state.selected_panel;
  <div className="panel d-flex flex-column border-right border-left h-25">
    {make_nav_pills(current, dispatch)}
    <div className="tab-content overflow-auto">
      <div className="tab-pane active">
        {switch (current) {
         | Some(Warnings) => <WarningView display={state.display} warnings={state.warnings} dispatch />
         | Some(DeadCode) => <DeadCodeView locations dispatch />
         | Some(Parameters) => <ParameterView parameters />
         | Some(Statistics) => <GvStatisticsView stats={state.stats} />
         | _ => React.null
         }}
      </div>
    </div>
  </div>;
};
