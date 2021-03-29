open Batteries;

module ToggledSet = Set.Make(String);

[@react.component]
let make = (~goblint: GvGoblint.solver_state) => {
  let (toggled, set_toggled) = React.useState(() => ToggledSet.empty);

  let toggle = (n, ()) => {
    set_toggled(t =>
      if (ToggledSet.mem(n, t)) {
        ToggledSet.remove(n, t);
      } else {
        ToggledSet.add(n, t);
      }
    );
  };

  <ul className="list-group list-group-flush">
    {goblint#global_names
     |> List.mapi((i, n) => {
          <li key={string_of_int(i)} className="list-group-item">
            <div className="d-flex justify-content-between align-items-center">
              {n |> React.string}
              <Button
                class_=["btn", "btn-sm", "dropdown-toggle"]
                color=`None
                on_click={toggle(n)}>
                React.null
              </Button>
            </div>
            {if (ToggledSet.mem(n, toggled)) {
               let analyses = goblint#global(n);
               switch (analyses) {
               | `Assoc(analyses) => <GvAnalysesView analyses />
               | _ => React.null
               };
             } else {
               React.null;
             }}
          </li>
        })
     |> React.list}
  </ul>;
};
