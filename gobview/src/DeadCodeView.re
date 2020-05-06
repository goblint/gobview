open SelectedView;

[@react.component]
let make = (~setFile, ~setFilepath, ~calls : list(Parse.call), ~setSelectedView, ~setLine) => {
  <div className="filebox">
    <h3>{ (List.length(calls) == 0 ? "No dead code available!" : "Dead Code") |> React.string}</h3>
    <ul>
      { calls |>
        List.mapi ( (i,c) => {
          <li className="cursor warnitem" key={string_of_int(i)} onClick={_ => { 
              setFile((_:string) =>Parse.get_file(c) |> Parse.get_file_from_filepath); 
              setFilepath((_:string) => Parse.get_file(c))
              setSelectedView((_:t) => Code)
              setLine((_:int) => int_of_string(Parse.get_line(c)))
            }
          }>
            { Parse.get_line(c) ++ " : " ++ Parse.get_file(c) |> React.string}
          </li>
      }) |> React.list }
    </ul>
  </div>
};
