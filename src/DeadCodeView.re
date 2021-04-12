open Batteries;

[@react.component]
let make = (~dispatch, ~locations) => {
  <div className="filebox">
    <h3>
      {(List.length(locations) == 0 ? "No dead code found!" : "Dead Code")
       |> React.string}
    </h3>
    <ul>
      {locations
       |> List.mapi((i, loc: Cil.location) => {
            <li
              className="cursor warnitem"
              key={string_of_int(i)}
              onClick={_ => {
                dispatch @@ `Set_file_name(loc.file);
                dispatch @@ `Set_file_path(loc.file);
                dispatch @@ `Set_line(loc.line);
              }}>
              {string_of_int(loc.line) ++ " : " ++ loc.file |> React.string}
            </li>
          })
       |> React.list}
    </ul>
  </div>;
};
