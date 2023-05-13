module ReactDOM = React.Dom

[@react.component]
let make = (~parameters) => {

    let params =
        parameters
        |> String.split_on_char(' ')
        |> List.map((s) => { String.sub(s, 1, String.length(s)-2)})
        |> String.concat(" ");

    let (history, setHistory) = React.useState(_ => [|params|]);
    let (value, setValue) = React.useState(_ => params);


    React.useEffect1(() => {
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

    let playButton = <Button on_click={on_submit}>
                         <IconPlay fill="bi bi-play-fill" />
                         {"Run" |> React.string}
                     </Button>;

    let map_history_entry_to_list_entry = (arr) => {
        arr |> Array.mapi((i, entry) =>
            {<li key={String.cat("params_", string_of_int(i))} className="list-group-item">
                {entry |> React.string}
            </li>}
        )
    };

    let list_elements = map_history_entry_to_list_entry(history);

    <div>
        <div className="input-group">
            {playButton}
            <Button color={`Danger} outline={true}>
                {"Cancel" |> React.string}
            </Button>
            <Input value on_change on_submit />
        </div>
        <div className="container-fluid text-center">
            <div className="row align-items-center">
                {"History" |> React.string}
            </div>
            <div className="row" style={ReactDOM.Style.make(~height="120px", ~maxHeight="100%", ~overflow="auto", ())}>
                <ol key={"params_list"} className="list-group">
                    {list_elements |> Array.to_list |> React.list}
                </ol>
            </div>
        </div>
    </div>;
}