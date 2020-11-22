open SelectedView;
open State;

[@react.component]
let make = (~dispatch, ~files) => {
  <div className="filebox">
    <h3> {"File list" |> React.string} </h3>
    <ul>
      {files
       |> List.filter(x => !Parse.file_is_empty(x))
       |> List.mapi((i, c) => {
            <li
              className="cursor"
              key={string_of_int(i)}
              onClick={_ => {
                dispatch @@ Set_file_name(Parse.file_to_name(c));
                dispatch @@ Set_file_path(Parse.file_to_path(c));
                dispatch @@ Set_selected_view(Content);
              }}>
              {Parse.file_to_path(c) |> React.string}
            </li>
          })
       |> React.list}
    </ul>
  </div>;
};
