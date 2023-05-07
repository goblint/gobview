[@react.component]
let make = (~parameters) => {

    let params =
        parameters
        |> String.split_on_char(' ')
        |> List.map((s) => { String.sub(s, 1, String.length(s)-2)})
        |> String.concat(" ");

    let (history, setHistory) = React.useState(_ => [|params|]);
    let (value, setValue) = React.useState(_ => params)

    let map_history_entry_to_list_entry = (arr) => {
        arr |> Array.mapi((i, entry) =>
            {<li key={String.cat("params_", string_of_int(i))} className="list-group-item">
                {entry |> React.string}
            </li>}
        )
    }

    let list_elements = map_history_entry_to_list_entry(history)

    React.useEffect1(() => {
        //list_elements = history |> map_history_entry_to_list_entry
        None
    }, [|history|]);

    React.useEffect1(() => {
        None
    }, [|value|]);

    let on_change = (new_value) => {
        setValue(_ => new_value)
    };

    let on_submit = () => {
        let new_history = Array.append(history, [|value|])
        setHistory(_ => new_history)

        // TODO transform param string with "' '" seperation mask
        // TODO execute newly transformed params
    };

    <div>
        //<div className="d-flex justify-content-between">
        //</div>
        <div className="container-fluid text-center">
            <div className="row">
                <Button on_click={on_submit}>
                    <IconPlay fill="bi bi-play-fill" />
                    {"Run" |> React.string}
                </Button>
                <Input value on_change on_submit />
            </div>
            <Button>
                {"Hello World" |> React.string}
            </Button>
            <div className="row align-items-center">
                {"History" |> React.string}
            </div>
            <div className="row">
                <ol key={"params_list"} className="list-group">
                    {list_elements |> Array.to_list |> React.list}
                </ol>
            </div>
        </div>
    </div>;
}