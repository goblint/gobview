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
       |> List.mapi((i, loc: GoblintCil.location) => {
            <li
              className="cursor warnitem"
              key={string_of_int(i)}
              onClick={_ => {
                dispatch @@ `DisplayFile(loc.file);
                dispatch @@ `InspectLine(GvInspect.Line.of_location(loc));
              }}>
              {string_of_int(loc.line) ++ " : " ++ loc.file |> React.string}
            </li>
          })
       |> React.list}
    </ul>
  </div>;
};
