[@react.component]
let make = (~globs) => {
  { globs |> 
    List.mapi ( (i,c) => {
      <div key={string_of_int(i)}>
        <ul id="myUL" >
          <TreeView tree={Parse.glob_to_tree(c)} />
        </ul>
        <hr />
      </div>
    }) |> React.list };
};
