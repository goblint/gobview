[@react.component]
let make = (~parameters) => {

    let params =
        parameters
        |> String.split_on_char(' ')
        |> List.map((s) => { String.sub(s, 1, String.length(s)-2)})
        |> String.concat(" ");

    let (history, setHistory) = React.useState(_ => [|params|]);

    let map_history_entry_to_list_entry = (arr) => {
        arr |> Array.map(entry => {
            <li className="list-group-item">
                {entry |> React.string}
            </li>
        })
    }

    let list_elements = map_history_entry_to_list_entry(history)

    React.useEffect1(() => {
        //list_elements = history |> map_history_entry_to_list_entry
        None
    }, [|history|]);

    let on_change = (_) => (); // not needed, but required...
    let on_submit = (_) => {
        let new_history = Array.append(history, [|"Hello World!"|])
        setHistory(_ => new_history)

        // TODO transform param string with "' '" seperation mask
        // TODO execute newly transformed params
    };

    <>
        //<div className="d-flex justify-content-between">
        //</div>
        <div className="container-fluid">
            <div className="row">
                <Button on_click={on_submit}>
                    <IconPlay fill="bi bi-play-fill" />
                    {"Run" |> React.string}
                </Button>
                <Input value=params on_change on_submit />
            </div>
            <div className="row align-items-center">
                {"History" |> React.string}
            </div>
            <div className="row">
                <ol className="list-group">
                    {list_elements |> Array.to_list |> React.list}
                </ol>
            </div>
        </div>
    </>;
}