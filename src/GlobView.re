[@react.component]
let make = (~globs) => {
  { globs |> Parse.glob_to_inverted_tree_2 |>
    List.mapi ( (i,c) => {
      <div key={string_of_int(i)}>
        <ul id="myUL" >
          <TreeView tree={c} />
        </ul>
        <hr />
      </div>
    }) |> React.list };
};
