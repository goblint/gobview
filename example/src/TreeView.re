[@react.component]
let make = (~tree) => {
    /* let (collapse, setCollapse) = React.useState(() => false); */
    let rec iter = (Tree.Node (s, c)) => 
        <li>
            <span key={s} className={List.length(c) > 0 ? "caret" : "" }>{s |> React.string}</span>  
            {
                if (List.length(c) > 0) {
                    <ul className="nested active">{List.map( iter, c) |> React.list} </ul>}
                else{
                    React.null
                }
            }
        </li>;
    <>
        {iter(tree)}
    </>;
};