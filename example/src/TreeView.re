let treex = Tree.Node ("Knoten", [Node ("swag", [Node ("Knoten", [Node ("swag", [Node ("Knoten", [Node ("swag", [Node ("Knoten", [Node ("swag", []), Node ("cool", [])])]), Node ("cool", [])])]), Node ("cool", [])])]), Node ("cool", [])]);

let treez = Parse.parse (Parse.xml_data )|> Parse.get_calls |> List.hd |> Parse.call_to_tree;




[@react.component]
let make = () => {
    let (collapse, setCollapse) = React.useState(() => false);
    let rec iter = (Tree.Node (s, c)) => 
        <li><span key={s} className={List.length(c) > 0 ? "caret" : "" } onClick={_ => {setCollapse( e => !e)} }>{s |> React.string}</span>  
            {if (List.length(c) > 0) {<ul style={ReactDOM.Style.make(
                ~display= collapse ? "none" : "block",(),)}> 
                {List.map( iter, c) |> React.list} </ul>}
            else
                {{React.null}}}
        </li>;
    <>
        <h2> {"Tree View" |> React.string}</h2>
        <ul>{iter(treez)}</ul>
    </>;
};