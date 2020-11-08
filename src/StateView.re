[@react.component]
let make = (~line, ~calls, ~filepath:string) => {
  let _ = filepath;
  let compare = (c) => { String.equal(Parse.get_file(c), filepath) };
  
  { calls |> List.filter (c => Parse.get_line(c) == string_of_int(line) && compare(c)) |>
    List.mapi ( (i,c) => {
      <div key={string_of_int(i)}>
        <ul id="myUL" >
          <TreeView tree={Parse.call_to_tree(c)} />
        </ul>
        <hr />
      </div>
    }) |> React.list };
};
