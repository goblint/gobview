open SelectedView;
open Reducer;

[@react.component]
let make = (~dispatch, ~calls: list(Parse.call)) => {
  <div className="filebox">
    <h3>
      {(List.length(calls) == 0 ? "No dead code found!" : "Dead Code")
       |> React.string}
    </h3>
    <ul>
      {calls
       |> List.mapi((i, c) => {
            <li
              className="cursor warnitem"
              key={string_of_int(i)}
              onClick={_ => {
                dispatch @@
                Set_file_name(
                  Parse.get_file(c) |> Parse.get_file_from_filepath,
                );
                dispatch @@ Set_file_path(Parse.get_file(c));
                dispatch @@ Set_selected_view(Content);
                dispatch @@ Set_line(int_of_string(Parse.get_line(c)));
              }}>
              {Parse.get_line(c)
               ++ " : "
               ++ Parse.get_file(c)
               |> React.string}
            </li>
          })
       |> React.list}
    </ul>
  </div>;
};
