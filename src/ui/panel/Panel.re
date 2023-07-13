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
              id={"nav-item-" ++ string_of_int(i)}
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
let make = (~state, ~dispatch, ~goblint_path, ~inputValue, ~setInputValue, ~disableRun, ~setDisableRun, ~inputState, ~setInputState, ~sortDesc, ~setSortDesc, ~history, ~setHistory, ~setAnalysisState) => {

  let locations = (state.goblint)#dead_locations;

  let current = state.selected_panel;

  let component = switch (current) {
    | Some(Warnings) => <WarningView warnings={state.warnings} dispatch />
    | Some(DeadCode) => <DeadCodeView locations dispatch />
    | Some(Statistics) => <GvStatisticsView stats={state.stats} />
    | Some(Parameters) => <ParameterView goblint_path inputValue setInputValue disableRun setDisableRun inputState setInputState sortDesc setSortDesc history setHistory setAnalysisState/>
    | _ => React.null
  };

  <div className="panel d-flex flex-column border-right border-left h-25">
    {make_nav_pills(current, dispatch)}
    <div className="tab-content overflow-auto">
      <div className="tab-pane active">
        {component}
      </div>
    </div>
  </div>;
};