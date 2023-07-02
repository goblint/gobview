open Lwt
open Cohttp
open Cohttp_lwt

module Js = Js_of_ocaml.Js
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

type inputState = Blacklisted | Malformed | Empty | Ok;

let inputState_to_string = (i: inputState) => {
    switch i {
        | Blacklisted => "Blacklisted options are not allowed";
        | Malformed => "Options are malformed. Check the input again"
        | Empty => "At least one parameter has to be entered";
        | Ok => "";
    }
}

let is_ok = (i: inputState) => i == Ok;

let option_whitelist = [
  "incremental.force-reanalyze.funs",
  "incremental.reluctant.enabled",
  "incremental.compare",
  "incremental.detect-renames",
  "incremental.restart",
  "incremental.postsolver",
  "annotation.goblint_precision"
];

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
let make = (~goblint_path, ~inputValue, ~setInputValue,~disableRun, ~setDisableRun, ~inputState, ~setInputState, ~sortDesc, ~setSortDesc, ~history, ~setHistory) => {
    // Linked to cancelation, see reasons below in on_cancel() for why it is commented out
    //let (disableCancel, setDisableCancel) = React.useState(_ => true);

    React.useEffect1(() => {
        None
    }, [|inputValue|]);

    React.useEffect1(() => {
        None
    }, [|sortDesc|]);

    let on_sort = (ev) => {
        React.Event.Mouse.preventDefault(ev);
        setSortDesc(_ => !sortDesc);
    }

    let get_input_state = (parameter_list: list((string, string)), is_malformed, input_val): inputState => {
        if (String.length(input_val) == 0) {
            Empty
        } else if(is_malformed || List.length(parameter_list) == 0) {
            Malformed
        } else {
            let has_found_option =
                parameter_list
                |> List.for_all(((option, _)) => {
                    let result =
                        option_whitelist
                        |> List.map (whitelisted_option => String.starts_with(~prefix=whitelisted_option, option))
                        |> List.find_opt (b => b == true);

                    switch (result) {
                        | Some(b) => b;
                        | None => false;
                    }
                });
            
            if (!has_found_option) {
                Blacklisted
            } else {
                Ok
            }
        }
    }

    let react_on_input = (parameter_list, is_malformed, inputValue) => {
        let input_state = get_input_state(parameter_list, is_malformed, inputValue);
        setInputState(_ => input_state);

        let isInvalid = !is_ok(input_state)
        setDisableRun(_ => isInvalid);

        isInvalid
    }

    let check_input = (inputValue) : bool => {
        let (tuple_parameter_list, is_malformed) =
            inputValue
            |> ParameterUtils.construct_parameters
            |> ((p,b)) => (p |> ParameterUtils.tuples_from_parameters, b);
    
        react_on_input(tuple_parameter_list, is_malformed, inputValue);
    }

    let on_add_parameter = (ev) => {
        let target = React.Event.Mouse.target(ev) |> ReactDOM.domElement_of_js;
        let unresolved_parameter = target##.textContent |> Js.Opt.to_option;

        let parameter = switch unresolved_parameter {
            | Some(p) => {
                let resolved_parameter = Js.to_string(p);
                if (String.length(inputValue) > 0) {
                    resolved_parameter ++ " "
                } else {
                    resolved_parameter
                }
            };
            | None => ""
        };
        
        setInputValue(inputVal => {
            let new_inputVal = String.cat(parameter, inputVal);
            let _ = check_input(new_inputVal);
            new_inputVal
        });
    }

    let on_change = (new_inputValue) => {
        let _ = check_input(new_inputValue);
        setInputValue(_ => new_inputValue);
    };

    let on_submit = () => {
        let (parameter_list, tuple_parameter_list, is_malformed) =
            inputValue
            |> ParameterUtils.construct_parameters
            |> ((p,b)) => (p, p |> ParameterUtils.tuples_from_parameters, b);

        // To prevent invalid default input to be executed, with i.e. blacklisted options, we check the input value first
        let isInvalid = react_on_input(tuple_parameter_list, is_malformed, inputValue);

        if (!isInvalid) {
            let time = Time.get_local_time();
            let element = (parameter_list, time, Executing);

            let new_history = List.cons(element, history);

            setHistory(_ => new_history);
            //setDisableCancel(_ => false);

            let modify_history = (result: paramState): unit => {
                let pickedElem = new_history |> List.hd;

                if (pickedElem == element) {
                    let intermediateHistory = new_history |> List.tl;
                    let new_history = List.cons(((parameter_list, time, result)), intermediateHistory);
                    setHistory(_ => new_history);
                    //setDisableCancel(_ => true);
                }
            }

            let inner_config_api_call = (config_body): Lwt.t(paramState) => {
                Client.post(config_uri, ~body=config_body,  ~headers=headers) >>=
                ((res, body)) => {
                    let code = res |> Response.status |> Code.code_of_status;
                    /*let msg_from_body = Cohttp_lwt.Body.to_string(body) >|= (body) => {
                        Lwt.return(body)
                    }*/
                    let _ = Body.drain_body(body);

                    if (code < 200 || code >= 400) {
                        Lwt.return(Error);
                    } else {
                        Lwt.return(Executed);
                    };
                };
            };

            let config_api_call = (config_opts: list((string, string))) => {
                if (List.length(config_opts) == 0) {
                    Lwt.return([Error])
                } else {
                    config_opts
                    |> List.map(((k,v)) => {
                        `List([`String(k), Yojson.Safe.from_string(v)])
                        |> Yojson.Safe.to_string
                        |> Body.of_string;
                    })
                    |> List.map(inner_config_api_call)
                    |> Lwt.npick;
                }
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
                tuple_parameter_list
                |> config_api_call >>=
                (result) => {
                    let result_state =
                        result
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

            ignore(analyze_api_call());
        }
    };

    // This cancel function is here in case it will be implemented in the http-server, not far fetched.
    /*let on_cancel = () => {
        let (param, time, _) = history |> List.hd;
        let intermediateHistory = history |> List.tl;
        let new_history = List.cons(((param, time, Canceled)), intermediateHistory);
        setHistory(_ => new_history);
        setDisableCancel(_ => true);
    };*/

    // Check whether default parameters are "Ok" or not
    let (tuple_parameter_list, is_malformed) =
        inputValue
        |> ParameterUtils.construct_parameters
        |> ((p,b)) => (p |> ParameterUtils.tuples_from_parameters, b);
    let _ = react_on_input(tuple_parameter_list, is_malformed, inputValue);

    let playButton = <Button disabled={disableRun} on_click={on_submit}>
                         <IconPlay />
                         {"Run" |> React.string}
                     </Button>;

    let map_history_entry_to_list_entry = (history) => {
        history
        |> (history) => {
            if (!sortDesc) {
                history |> List.rev
            } else {
                history
            }
        }
        |> List.mapi((i, (parameter_grouping, time, paramState)) =>
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
                                <span key={"pill_" ++ string_of_int(j)} className="m-1 badge rounded-pill bg-secondary text" style={ReactDOM.Style.make(~cursor="pointer", ())} onClick=on_add_parameter>
                                    {e |> React.string}
                                </span>
                            }) |> React.list}
                        </div>
                    </div>
                </div>
            </li>}
        );
    };

    let list_elements = history |> map_history_entry_to_list_entry;

    <div>
        <div className="input-group mb-2 has-validation">
            {playButton}
            
            // Commented out because http server does not support cancelation yet
            /*<Button color={`Danger} outline={true} on_click={on_cancel} disabled={disableCancel}>
                {"Cancel" |> React.string}
            </Button>*/

            <label data="tooltip" title=goblint_path className="input-group-text" type_="tooltip_path">{"./goblint" |> React.string}</label>
            // Input and tooltip are seperated due to display issues
            {switch inputState {
                | Malformed
                | Blacklisted
                | Empty => <Input class_=["form-control", "is-invalid"] value=inputValue on_change on_submit key="tooltip_path" />
                | Ok => <Input value=inputValue on_change on_submit key="tooltip_path" /*id=input_id*/ />;
            }}
            {switch inputState {
                | Ok => React.null;
                | _ => {
                    <div className="invalid-tooltip">
                        {inputState |> inputState_to_string |> React.string}
                    </div>
                };
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
                                 <div className="col-2" onClick=on_sort style={ReactDOM.Style.make(~cursor="pointer", ())}>
                                    {"Time " |> React.string}
                                    {switch sortDesc {
                                    | true => <IconArrowUp />
                                    | _ => <IconArrowDown />
                                    };
                                    }
                                 </div>
                                 <div className="col">
                                     {"Parameters" |> React.string}
                                 </div>
                             </div>
                         </div>
                     </li>}
                    {list_elements |> React.list}
                </ol>
            </div>
        </div>
    </div>;
}