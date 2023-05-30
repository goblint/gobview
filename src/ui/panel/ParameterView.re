open Lwt
open Cohttp
open Cohttp_lwt

module Client = Cohttp_lwt_jsoo.Client
module ReactDOM = React.Dom

type paramState = Executed | Executing | Canceled | Error;

// For debugging purposes and potential further use
let paramState_to_string = (p: paramState) => {
    switch (p) {
        | Executed => "Executed";
        | Executing => "Executing";
        | Canceled => "Canceled";
        | Error => "Error";
    };
};

[@react.component]
let make = (~parameters, ~history, ~setHistory) => {

    let (value, setValue) = React.useState(_ => parameters);
    let (disableCancel, setDisableCancel) = React.useState(_ => true);

    React.useEffect1(() => {
        None
    }, [|value|]);

    let on_change = (new_value) => {
        setValue(_ => new_value);
    };

    let on_submit = () => {
        history |> Array.length |> string_of_int |> Util.log;

        let time = Time.getLocalTime();
        let state = ref(Executing);
        let element = (value, time, state.contents);

        let new_history = [|element|] |> Array.append(history);
        setHistory(_ => new_history);
        setDisableCancel(_ => false);

        let /*parameterList*/ _ = ParameterUtils.constructParameters(value);
        let headers = Header.init_with("Content-Type", "application/json");
        let body = "{
                    \"jsonrpc\": \"2.0\",
                    \"id\": \"5\",
                    \"method\": \"analyze\",
                    \"params\": {\"reset\": false}
                    }"
                    |> Body.of_string;

        let scheme = "http";
        let host = "localhost";
        let port = 8080;
        let path = "/api/analyze";

        let uri = Printf.sprintf("%s://%s:%d%s", scheme, host, port, path) |> Uri.of_string;

        let new_state = Client.post(uri, ~body=body, ~headers=headers) >>= ((res, _)) => {
            let code = res |> Response.status |> Code.code_of_status

            if (code < 200 || code >= 400) {
                Lwt.return(Error)
            } else {
                Lwt.return(Executed)
            };
        };

        let res_state = switch (new_state |> Lwt.poll) {
            | Some(p) => p
            | None => Error
        };

        let lastElemIndexInHistory = Array.length(new_history) - 1;
        let lastElement = lastElemIndexInHistory |> Array.get(new_history);

        if (element == lastElement) {
            let intermediateHistory = lastElemIndexInHistory |> Array.sub(new_history, 0);

            let new_element = (value, time, res_state);

            let new_history = [|new_element|] |> Array.append(intermediateHistory);
            setHistory(_ => new_history);
            setDisableCancel(_ => true);
        }
        
    };

    let on_cancel = () => {
        let lastElemIndex = Array.length(history) - 1;
        let (param, time, _) = Array.get(history, lastElemIndex);

        let intermediateHistory = Array.sub(history, 0, lastElemIndex);
        let new_history = Array.append(intermediateHistory, [|(param, time, Canceled)|]);
        setHistory(_ => new_history);

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

    let list_elements = history |> map_history_entry_to_list_entry;

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