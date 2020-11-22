open SelectedView;
open State;

[@react.component]
let make = (~dispatch, ~warnings: list(Parse.warning)) => {
  <div className="filebox">
    <h3>
      {(List.length(warnings) == 0 ? "No warnings found!" : "Warnings")
       |> React.string}
    </h3>
    <ul>
      {warnings
       |> List.mapi((i, c) => {
            <li
              className="cursor warnitem"
              key={string_of_int(i)}
              onClick={_ => {
                dispatch @@
                Set_file_name(
                  Parse.warning_to_file(c) |> Parse.get_file_from_filepath,
                );
                dispatch @@ Set_file_path(Parse.warning_to_file(c));
                dispatch @@ Set_selected_view(Content);
                dispatch @@
                Set_line(int_of_string(Parse.warning_to_line(c)));
              }}>
              <b> {Parse.warning_to_text(c) |> React.string} </b>
              <br />
              {Parse.warning_to_line(c)
               ++ " : "
               ++ Parse.warning_to_file(c)
               |> React.string}
            </li>
          })
       |> React.list}
    </ul>
  </div>;
};
