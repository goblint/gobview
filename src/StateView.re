open State;

[@react.component]
let make = (~state, ~calls) => {
  calls
  |> (
    switch (state.selected_view) {
    | Content => List.filter(c => Parse.get_id(c) == string_of_int(state.id))
    | _ =>
      List.filter(c =>
        Parse.get_file(c) == state.file_path
        && Parse.get_line(c) == string_of_int(state.line)
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
