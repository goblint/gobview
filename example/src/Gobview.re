let treez = Parse.parse (Parse.xml_data )|> Parse.get_calls |> List.hd |> Parse.call_to_tree;

[@react.component]
let make = () => {
  <ul id="myUL"><TreeView tree=treez /></ul>
};
