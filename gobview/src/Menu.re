open SelectedView;

let items = [(Code, "Code View"), (Node, "Node View"), (Warning, "Warnings"), (DeadCode, "Dead Code"),
    (File, "Select File"), (Parameters, "Parameters"), (Compare, "Compare States"), (Statistics, "Statistics")];

[@react.component]
let make = (~selectedView, ~setSelectedView) => {

    <div className="menu">
        { items |> List.mapi ( (i,(stype, stype_string)) => {
            <span key={string_of_int(i)} className={"menuitem " ++ (selectedView == stype ? "menuitemSelected " : "")} 
                onClick={_ => setSelectedView((_:t) => stype)} >
                { stype_string |> React.string}
            </span>
        }) |> React.list }
    </div>
}
