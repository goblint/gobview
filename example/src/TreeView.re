let treex = Tree.Node ("Knoten", [Node ("swag", [Node ("Knoten", [Node ("swag", [Node ("Knoten", [Node ("swag", [Node ("Knoten", [Node ("swag", []), Node ("cool", [])])]), Node ("cool", [])])]), Node ("cool", [])])]), Node ("cool", [])]);

let treez = Parse.parse (Parse.xml_data )|> Parse.get_calls |> List.hd |> Parse.call_to_tree


let rec iter = (Tree.Node (s, c)) => <ul><li><span key={s}>{s |> React.string}</span> {List.map( iter, c) |> React.list}</li></ul>;

[@react.component]
let make = () => {
    <>
        <h2> {"Tree View" |> React.string}</h2>
        {iter(treez)}
    </>;
};