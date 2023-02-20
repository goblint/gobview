open Batteries;

[@react.component]
let make = (~dispatch, ~locations) => {
  <div className="filebox">
    {
      if (List.length(locations) == 0)
        {<p>{"No dead code found!" |> React.string}</p>}
      else {
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
      }}
  </div>;
};
