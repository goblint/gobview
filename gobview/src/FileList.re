[@react.component]
let make = (~files,~setFile, ~setFilepath) => {
  <div className="filebox">
    <h3>{"File list" |> React.string}</h3>
    <ul>
      { files |> List.filter((x => !Parse.file_is_empty(x))) |>
        List.mapi ( (i,c) => {
          <li className="cursor" key={string_of_int(i)} onClick={_ => { 
              setFile((_:string) =>Parse.file_to_name(c)); 
              setFilepath((_:string) => Parse.file_to_path(c))}
          }>
            { Parse.file_to_path(c)|> React.string}
          </li>
      }) |> React.list }
    </ul>
  </div>
};
