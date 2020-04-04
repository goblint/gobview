let calls = Parse.parse (Parse.xml_data )|> Parse.get_calls;
let treez = calls |> List.hd |> Parse.call_to_tree;
let tree_list = List.map (Parse.call_to_tree , calls);

[@react.component]
let make = () => {
  {tree_list |> 
    List.mapi ( (i,l) => {
      <div key={string_of_int(i)}>
        <ul id="myUL" >
          <TreeView tree=l />
        </ul>
      </div>
    }) |> React.list }
};
