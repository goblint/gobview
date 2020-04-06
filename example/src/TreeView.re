[@react.component]
let make = (~tree) => {
    /* let (collapse, setCollapse) = React.useState(() => false); */
    let rec iter = (i, Tree.Node (s, c)) => 
        <li key={string_of_int(i)}>
            <span  className={List.length(c) > 0 ? "caret" : "" }>{s |> React.string}</span>  
            {
                if (List.length(c) > 0) {
                    <ul className={"nested " ++ (s == "context" ? "" : "active")}>{List.mapi( iter, c) |> React.list} </ul>}
                else{
                    React.null
                }
            }
        </li>;
    <>
        {iter(0, tree)}
    </>;
};