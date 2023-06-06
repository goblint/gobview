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

let scheme = "http";
let host = "127.0.0.1";
let port = 8000;
let analyze_path = "/api/analyze";
let config_path = "/api/config";
let analyze_uri = Printf.sprintf("%s://%s:%d%s", scheme, host, port, analyze_path) |> Uri.of_string;
let config_uri = Printf.sprintf("%s://%s:%d%s", scheme, host, port, config_path) |> Uri.of_string;

let headers = [
    ("Content-Type", "application/json"),
    ("Access-Control-Allow-Origin", Printf.sprintf("%s://%s:%d", scheme, host, port)),
    ("Access-Control-Allow-Headers", "Content-Type"),
    ("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS")
] |> Header.of_list;

[@react.component]
let make = (~goblint_path, ~parameters, ~history, ~setHistory) => {

    let (value, setValue) = React.useState(_ => parameters);
    let (disableCancel, setDisableCancel) = React.useState(_ => true);

    React.useEffect1(() => {
        None
    }, [|value|]);

    let on_change = (new_value) => {
        setValue(_ => new_value);
    };

    let on_submit = () => {
        let time = Time.get_local_time();
        let element = (value, time, Executing);

        let new_history = [|element|] |> Array.append(history);
        setHistory(_ => new_history);
        setDisableCancel(_ => false);

        let parameter_list = 
            value
            |> ParameterUtils.construct_parameters
            |> ParameterUtils.tuples_from_parameters;

        // TODO create config body and request per created tuple in parameter_list
        let config_body =
            `Tuple (parameter_list |> List.map(s => `String (s))) // WIP, does not compile here
            |> Yojson.Safe.to_string
            |> Body.of_string;

        let analyze_body = `List([
         `String ("Functions"),
         `List ([])
        ])|> Yojson.Safe.to_string |> Body.of_string;

        // config endpoint
        let config_res = Client.post(config_uri, ~body=/*config_body*/`Empty,  ~headers=headers) >>= ((res, _)) => {
            let code = res |> Response.status |> Code.code_of_status

            if (code < 200 || code >= 400) {
                Lwt.return(Error)
            } else {
                Lwt.return(Executed)
            };
        }

        let res_state = ref(switch (config_res |> Lwt.poll) {
            | Some(p) => p
            | None => Error
        });

        if (res_state.contents != Error && res_state.contents != Canceled) {

            // analyze endpoint
            let new_state = Client.post(analyze_uri, ~body=analyze_body, ~headers=headers) >>= ((res, _)) => {
                let code = res |> Response.status |> Code.code_of_status

                if (code < 200 || code >= 400) {
                    Lwt.return(Error)
                } else {
                    Lwt.return(Executed)
                };
            };

            res_state := switch (new_state |> Lwt.poll) {
                | Some(p) => p
                | None => Error
            };

        }

        let lastElemIndexInHistory = Array.length(new_history) - 1;
        let lastElement = lastElemIndexInHistory |> Array.get(new_history);

        if (element == lastElement) {
            let intermediateHistory = lastElemIndexInHistory |> Array.sub(new_history, 0);

            let new_element = (value, time, res_state.contents);

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

    // TODO Show real goblint path in tooltip
    <div>
        <div className="input-group mb-2">
            {playButton}
            <Button color={`Danger} outline={true} on_click={on_cancel} disabled={disableCancel}>
                {"Cancel" |> React.string}
            </Button>
            <label data="tooltip" title=goblint_path className="input-group-text" type_="inputGroupFile01">{"./goblint" |> React.string}</label>
            <Input key="inputGroupFile01" value on_change on_submit />
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