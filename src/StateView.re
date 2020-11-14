[@react.component]
let make =
    (~selectedView: SelectedView.t, ~id, ~line, ~calls, ~filepath: string) => {
  calls
  |> (
    switch (selectedView) {
    | Node => List.filter(c => Parse.get_id(c) == string_of_int(id))
    | _ =>
      List.filter(c =>
        Parse.get_file(c) == filepath
        && Parse.get_line(c) == string_of_int(line)
      )
    }
  )
  |> List.mapi((i, c) => {
       <div key={string_of_int(i)}>
         <ul id="myUL"> <TreeView tree={Parse.call_to_tree(c)} /> </ul>
         <hr />
       </div>
     })
  |> React.list;
};
