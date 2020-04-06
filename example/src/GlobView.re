let globs = Parse.parse (Parse.xml_data )|> Parse.get_globs ;

[@react.component]
let make = () => {
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
