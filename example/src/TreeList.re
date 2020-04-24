[@react.component]
let make = (~line, ~calls, ~filepath) => {
  { calls |> List.filter (c => Parse.get_line(c) == string_of_int(line) && String.equal(Parse.get_file(c), filepath)) |>
    List.mapi ( (i,c) => {
      <div key={string_of_int(i)}>
        <ul id="myUL" >
          <TreeView tree={Parse.call_to_tree(c)} />
        </ul>
        <hr />
      </div>
    }) |> React.list };
};
