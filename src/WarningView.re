open Batteries;

[@react.component]
let make = (~warnings, ~dispatch) => {
  <div className="filebox">
    <h3>
      {(List.length(warnings) == 0 ? "No warnings found!" : "Warnings")
       |> React.string}
    </h3>
    <ul>
      {warnings
       |> List.map(State.Warning.to_list)
       |> List.flatten
       |> List.mapi((i, (text, loc: Cil.location)) => {
            <li
              className="cursor warnitem"
              key={string_of_int(i)}
              onClick={_ => {
                dispatch @@ `Set_file_name(loc.file);
                dispatch @@ `Set_file_path(loc.file);
                dispatch @@ `Set_line(loc.line);
              }}>
              <b> {text |> React.string} </b>
              <br />
              {string_of_int(loc.line) ++ " : " ++ loc.file |> React.string}
            </li>
          })
       |> React.list}
    </ul>
  </div>;
};
