open State;

let views = [
  (Selected_panel.Warnings, "Warnings"),
  (Selected_panel.Dead_code, "Dead code"),
  (Selected_panel.Parameters, "Parameters"),
  (Selected_panel.Statistics, "Statistics"),
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
              onClick={on_click(`Switch_panel(Some(v)))}>
              {n |> React.string}
            </a>
          </li>
        })
     |> React.list}
    <li className="nav-item">
      <a
        href="#"
        className="nav-link"
        onClick={on_click(`Switch_panel(None))}>
        {"✖" |> React.string}
      </a>
    </li>
  </ul>;
};

[@react.component]
let make = (~state, ~dispatch) => {
  let parameters =
    switch (Yojson.Safe.Util.member("command", state.meta)) {
    | `String(command) => command
    | _ => ""
    };

  let to_parse_warning = ((s, loc: Cil.location)) =>
    Parse.Warning(loc.file, string_of_int(loc.line), s);
  let warnings =
    state.warnings
    |> List.map(w =>
         switch (w) {
         | `text(w) => [to_parse_warning(w)]
         | `group(_, l) => List.map(to_parse_warning, l)
         }
       )
    |> List.concat;

  let current = selected_panel_opt(state);
  <div className="border-right border-left">
    {make_nav_pills(current, dispatch)}
    <div className="tab-content">
      <div className="tab-pane active">
        {switch (current) {
         | Some(Selected_panel.Warnings) => <WarningView warnings dispatch />
         | Some(Selected_panel.Dead_code) =>
           <DeadCodeView
             calls={
               state.pdata
               |> Parse.get_calls
               |> List.filter(Parse.has_dead_code)
               |> Parse.sort_calls_by_line
             }
             dispatch
           />
         | Some(Selected_panel.Parameters) => <ParameterView parameters />
         | Some(Selected_panel.Statistics) =>
           <StatisticsView statistics={state.pdata |> Parse.get_statistics} />

         | _ => React.null
         }}
      </div>
    </div>
  </div>;
};
