open SelectedView;

[@react.component]
let make = (~setFile, ~setFilepath, ~warnings : list(Parse.warning), ~setSelectedView, ~setLine) => {
  <div className="filebox">
    <h3>{ (List.length(warnings) == 0 ? "No warnings available!" : "Warnings") |> React.string}</h3>
    <ul>
      { warnings |>
        List.mapi ( (i,c) => {
          <li className="cursor warnitem" key={string_of_int(i)} onClick={_ => { 
              setFile((_:string) =>Parse.warning_to_file(c) |> Parse.get_file_from_filepath); 
              setFilepath((_:string) => Parse.warning_to_file(c))
              setSelectedView((_:t) => Code)
              setLine((_:int) => int_of_string(Parse.warning_to_line(c)))
            }
          }>
          <b>{ Parse.warning_to_text(c)|> React.string}</b>
          <br />
          { Parse.warning_to_line(c) ++ " : " ++ Parse.warning_to_file(c) |> React.string}
            
          </li>
      }) |> React.list }
    </ul>
  </div>
};
