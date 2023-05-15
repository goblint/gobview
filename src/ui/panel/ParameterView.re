open Unix
//open Lwt
//open Cohttp
//open Cohttp_lwt_unix
// TODO use above mentioned packages for http client to call goblint http server

module ReactDOM = React.Dom

type paramState = Executed | Executing | Canceled | Error;

[@react.component]
let make = (~parameters) => {

    let initParams =
        parameters
        |> String.split_on_char(' ')
        |> List.map((s) => { String.sub(s, 1, String.length(s)-2)})
        |> String.concat(" ");

    let timeToString = (time) => {
        string_of_int(time.tm_min)
        |> String.cat(":")
        |> String.cat(string_of_int(time.tm_hour))
        |> String.cat(" ");
    }

    let getLocalTime = () => {
        Unix.time()
        |> Unix.localtime
        |> timeToString;
    }

    let (history, setHistory) = React.useState(_ => [|(initParams, getLocalTime(), Executed)|]);
    let (value, setValue) = React.useState(_ => initParams);
    let (disableCancel, setDisableCancel) = React.useState(_ => true);

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
        let newHistory = Array.append(history, [|(value, getLocalTime(), Executing)|])
        setHistory(_ => newHistory)

        // TODO transform param string with "' '" seperation mask
        // TODO execute newly transformed params

        // TODO use cohttp to call goblint server

        setDisableCancel(_ => false);
    };

    let on_cancel = () => {
        let lastElemIndex = Array.length(history) - 1;
        let (param, time, _) = Array.get(history, lastElemIndex);

        let intermediateHistory = Array.sub(history, 0, lastElemIndex);
        let newHistory = Array.append(intermediateHistory, [|(param, time, Canceled)|]);
        setHistory(_ => newHistory);

        setDisableCancel(_ => true);
    }

    let playButton = <Button on_click={on_submit}>
                         <IconPlay fill="bi bi-play-fill" />
                         {"Run" |> React.string}
                     </Button>;

    let map_history_entry_to_list_entry = (arr) => {
        arr |> Array.mapi((i, (entry, time, paramState)) =>
            {<li key={String.cat("params_", string_of_int(i))} className="list-group-item">
                <div className="container text-center">
                    <div className="row">
                        <div className="col-2">
                            {switch paramState {
                            | Executing => <Spinner />
                            | Canceled  => <IconX />
                            | Executed  => <IconCheckmark />
                            | Error     => <IconWarning />
                            }}
                        </div>
                        <div className="col-2">
                            <IconClock />
                            {time |> React.string}
                        </div>
                        <div className="col">
                            {entry |> React.string}
                        </div>
                    </div>
                </div>
            </li>}
        )
    };

    let list_elements = map_history_entry_to_list_entry(history);

    <div>
        <div className="input-group mb-2">
            {playButton}
            <Button color={`Danger} outline={true} on_click={on_cancel} disabled={disableCancel}>
                {"Cancel" |> React.string}
            </Button>
            <Input value on_change on_submit />
        </div>
        <div className="container-fluid text-center">
            <div className="row align-items-center">
                {"History" |> React.string}
            </div>
            <div className="row" style={ReactDOM.Style.make(~height="115px", ~maxHeight="100%", ~overflow="auto", ())}>
                <ol key={"params_list"} className="list-group">
                    {<li key={"params_header_item"} className="list-group-item">
                         <div className="container text-center">
                             <div className="row">
                                 <div className="col-2">
                                     {"Status" |> React.string}
                                 </div>
                                 <div className="col-2">
                                     {"Time" |> React.string}
                                 </div>
                                 <div className="col">
                                     {"Parameters" |> React.string}
                                 </div>
                             </div>
                         </div>
                     </li>}
                    {list_elements |> Array.to_list |> React.list}
                </ol>
            </div>
        </div>
    </div>;
}