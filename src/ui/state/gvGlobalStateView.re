open Batteries;

module ToggledSet = Set.Make(String);

[@react.component]
let make = (~analyses) => {
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
    {analyses
     |> List.mapi((i, (n, results)) => {
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
               <GvAnalysesView results />;
             } else {
               React.null;
             }}
          </li>
        })
     |> React.list}
  </ul>;
};
