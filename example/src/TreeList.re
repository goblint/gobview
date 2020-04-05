let calls = Parse.parse (Parse.xml_data )|> Parse.get_calls;

[@react.component]
let make = (~line) => {
  { calls |> 
    List.filter( call => {Parse.get_line(call) == string_of_int(line)}) |>
    List.mapi ( (i,c) => {
      <div key={string_of_int(i)}>
        <ul id="myUL" >
          <TreeView tree={Parse.call_to_tree(c)} />
        </ul>
        <hr />
      </div>
    }) |> React.list };
};
