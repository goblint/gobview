[@react.component]
let make = (~line, ~calls) => {
  { calls |> 
    List.mapi ( (i,c) => {
      <div key={string_of_int(i)} className={(Parse.get_line(c) == string_of_int(line)) ? "active" : "nested"}>
        <ul id="myUL" >
          <TreeView tree={Parse.call_to_tree(c)} />
        </ul>
        <hr />
      </div>
    }) |> React.list };
};
