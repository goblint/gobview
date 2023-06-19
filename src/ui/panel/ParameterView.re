open Lwt
open Cohttp
open Cohttp_lwt

module Client = Cohttp_lwt_jsoo.Client
module ReactDOM = React.Dom

type paramState = Executed | Executing | Canceled | Error;

// For debugging purposes and potential further use
let paramState_to_string = (p: paramState) => {
    switch p {
        | Executed => "Executed";
        | Executing => "Executing";
        | Canceled => "Canceled";
        | Error => "Error";
    };
};

let is_successful = (p: paramState) => p == Executed;

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

let rev_arr = (array) => array |> Array.to_list |> List.rev |> Array.of_list;

[@react.component]
let make = (~goblint_path, ~parameters, ~history, ~setHistory) => {

    let (value, setValue) = React.useState(_ => parameters |> ParameterUtils.concat_parameter_list);
    // Linked to cancelation, see reasons below for why it is commented out
    //let (disableCancel, setDisableCancel) = React.useState(_ => true);
    let (disableRun, setDisableRun) = React.useState(_ => false);
    let (sortDesc, setSortDesc) = React.useState(_ => true);
    let (hasServerOpts, setHasServerOpts) = React.useState(_ => false);

    React.useEffect1(() => {
        None
    }, [|value|]);

    React.useEffect1(() => {
        setHistory(_ => history |> rev_arr);
        None
    }, [|sortDesc|]);

    let on_sort = (ev) => {
        React.Event.Mouse.preventDefault(ev);
        setSortDesc(_ => !sortDesc);
    }


    let on_change = (new_value) => {
        let server_opt_regex = Str.regexp_string("server.");
        let server_opts_found = switch (Str.search_forward(server_opt_regex, new_value, 0)) {
            | exception Not_found => false;
            | _ => true
        }
        setHasServerOpts(_ => server_opts_found);
        setDisableRun(_ => server_opts_found);

        setValue(_ => new_value);
    };

    let on_submit = () => {
        if (!hasServerOpts) {
            let parameter_list = 
                value
                |> ParameterUtils.construct_parameters
                |> ParameterUtils.group_parameters
                
            let time = Time.get_local_time();
            let element = (parameter_list, time, Executing);

            let new_history = if (sortDesc) {
                history |> Array.append([|element|])
            } else {
                [|element|] |> Array.append(history)
            };

            setHistory(_ => new_history);
            //setDisableCancel(_ => false);

            let modify_history = (result: paramState): unit => {
                let lastIndex = Array.length(new_history) - 1;
                // This tuple is used to calculate where to update the last added history/parameter element 
                let (index, startIndex, endIndex) = if (sortDesc) {
                    (0, 1, lastIndex)
                } else {
                    (lastIndex, 0, lastIndex)
                };

                let pickedElem = index |> Array.get(new_history);

                if (pickedElem == element) {
                    let intermediateHistory = endIndex |> Array.sub(new_history, startIndex);

                    let new_element = (parameter_list, time, result);

                    let new_history = if (sortDesc) {
                        intermediateHistory |> Array.append([|new_element|])
                    } else {
                        [|new_element|] |> Array.append(intermediateHistory)
                    };

                    setHistory(_ => new_history);
                    //setDisableCancel(_ => true);
                }
            }

            let inner_config_api_call = (config_body): Lwt.t(paramState) => {
                Client.post(config_uri, ~body=config_body,  ~headers=headers) >>=
                ((res, body)) => {
                    let code = res |> Response.status |> Code.code_of_status;
                    let _ = Body.drain_body(body);

                    if (code < 200 || code >= 400) {
                        Lwt.return(Error);
                    } else {
                        Lwt.return(Executed);
                    };
                };
            };

            let config_api_call = (config_opts: list((string, string))) => {
                config_opts
                |> List.map(((k,v)) => {
                    `List([`String(k), `String(v)])
                    |> Yojson.Safe.to_string
                    |> Body.of_string;
                })
                |> List.map(inner_config_api_call)
                |> Lwt.npick;
            };

            let inner_analyze_api_call = (analyze_body): Lwt.t(paramState) => {
                Client.post(analyze_uri, ~body=analyze_body,  ~headers=headers) >>=
                ((res, body)) => {
                    let code = res |> Response.status |> Code.code_of_status;
                    let _ = Body.drain_body(body);

                    if (code < 200 || code >= 400) {
                        Lwt.return(Error);
                    } else {
                        Lwt.return(Executed);
                    };
                };
            };

            let analyze_body =
                `List([`String ("Functions"), `List ([])])
                |> Yojson.Safe.to_string
                |> Body.of_string;

            let analyze_api_call = () => {
                let config_opts = parameter_list |> ParameterUtils.tuples_from_parameters;

                config_opts
                |> config_api_call >>=
                (result) => {
                    let result_state = result
                    |> List.map(is_successful)
                    |> List.fold_left((a,b) => a && b, true)
                    |> ((s) => if (s) { Executed } else { Error });

                    Lwt.return(result_state);
                } >>=
                (result) => {
                    switch result {
                    | Executed => inner_analyze_api_call(analyze_body);
                    | _ => Lwt.return(Error);
                    }
                } >>=
                (result) => {
                    modify_history(result);
                    Lwt.return()
                };
            };

            let () = analyze_api_call() |> ignore;
        
        }
    };

    // This cancel function is here in case it will be implemented in the http-server, not far fetched.
    /*let on_cancel = () => {
        let lastElemIndex = Array.length(history) - 1;
        let (param, time, _) = Array.get(history, lastElemIndex);

        let intermediateHistory = Array.sub(history, 0, lastElemIndex);
        let new_history = Array.append(intermediateHistory, [|(param, time, Canceled)|]);
        setHistory(_ => new_history);

        setDisableCancel(_ => true);
    };*/

    let playButton = <Button disabled={disableRun} on_click={on_submit}>
                         <IconPlay />
                         {"Run" |> React.string}
                     </Button>;

    let map_history_entry_to_list_entry = (arr) => {
        arr |> Array.mapi((i, (parameter_grouping, time, paramState)) =>
            {<li key={"params_" ++ string_of_int(i)} className="list-group-item">
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
                            <div className="ms-2">
                                <IconClock />
                                {time ++ " " |> React.string}
                            </div>
                        </div>
                        <div className="col">
                            {parameter_grouping |> List.mapi((j,e) => {
                                <span key={"pill_" ++ string_of_int(j)} className="m-1 badge rounded-pill bg-secondary text">{e |> React.string}</span>
                            }) |> React.list}
                        </div>
                    </div>
                </div>
            </li>}
        );
    };

    let list_elements = history |> map_history_entry_to_list_entry;

    let icon_for_sort_dir = if (sortDesc) {
        <IconArrowUp on_click=on_sort />
    } else {
        <IconArrowDown on_click=on_sort />
    };

    <div>
        <div className="input-group mb-2 has-validation">
            {playButton}
            
            // Commented out because http server does not support cancelation yet
            /*<Button color={`Danger} outline={true} on_click={on_cancel} disabled={disableCancel}>
                {"Cancel" |> React.string}
            </Button>*/

            <label data="tooltip" title=goblint_path className="input-group-text" type_="tooltip_path">{"./goblint" |> React.string}</label>
            // Input and tooltip are seperated due to display issues
            {switch hasServerOpts {
                | true => <Input class_=["form-control", "is-invalid"] value on_change on_submit key="tooltip_path" /*style={ReactDOM.Style.make(~maxWidth="100%", ())}*//>
                | false => <Input value on_change on_submit key="tooltip_path" />;
            }}
            {switch hasServerOpts {
                | true => {
                    <div className="invalid-tooltip">
                        {"Server options are not allowed" |> React.string}
                    </div>
                };
                | _ => React.null;
            }}
        </div>

        <div className="container-fluid text-center">
            <div className="row" style={ReactDOM.Style.make(~height="140px", ~maxHeight="100%", ~overflow="auto", ())}>
                <ol key={"params_list"} className="list-group">
                    {<li key={"params_header_item"} className="list-group-item">
                         <div className="container text-center">
                             <div className="row">
                                 <div className="col-2">
                                    {"Status" |> React.string}
                                 </div>
                                 <div className="col-2">
                                    {"Time " |> React.string}
                                    {icon_for_sort_dir}
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