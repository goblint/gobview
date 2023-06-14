open State

let concat_parameters = (parameters) => {
    parameters
    |> String.split_on_char(' ')
    |> List.map((s) => { String.sub(s, 1, String.length(s)-2)})
    |> String.concat(" ")
};

let concat_parameter_list = String.concat(" ");

let get_parameters = (state) => {
    let raw_params = switch (Yojson.Safe.Util.member("command", state.meta)) {
        | `String(command) => command
        | _ => ""
    }
    |> String.split_on_char(' ')
    |> List.map((s) => { String.sub(s, 1, String.length(s)-2)});

    (raw_params |> List.hd, raw_params |> List.tl)
};
 
let construct_parameters = (parameters) => parameters |> String.split_on_char(' ');//|> List.map((s) => "'" |> String.cat(s) |> String.cat("'"))

let rec group_parameters = (parameters: list(string)): list(string) => {
    if (parameters |> List.length > 0) {
        
        let (command, tail) = (parameters |> List.hd, parameters |> List.tl);
        switch command {
            | "--set" => {
                let (option, tail') = (tail |> List.hd, tail |> List.tl);
                let (value, tail'') = (tail' |> List.hd, tail' |> List.tl);

                let parameter = Printf.sprintf("--set %s %s", option, value);
                tail'' |> group_parameters |> List.cons(parameter)
            }
            | "--enable" => {
                let (option, tail') = (tail |> List.hd, tail |> List.tl);

                let parameter = Printf.sprintf("--enable %s", option);
                tail' |> group_parameters |> List.cons(parameter)
            } 
            | "--disable" => {
                let (option, tail') = (tail |> List.hd, tail |> List.tl);

                let parameter = Printf.sprintf("--disable %s", option);
                tail' |> group_parameters |> List.cons(parameter)
            }
            | _ => []
        }
    } else {
        []
    }
}

let contains_empty_string = (string_list) =>
    string_list
    |> List.map(String.length)
    |> List.filter(i => i == 0)
    |> List.length > 0

let tuples_from_parameters = (parameters) => {
    parameters
    |> List.map(params => {
        let params_split = params |> String.split_on_char(' ');
        let command = params_split |> List.hd;
        let param' = params_split |> List.tl;

        switch command {
            | "--set" => {
                let (option, tail') = (param' |> List.hd, param' |> List.tl);
                let value = tail' |> List.hd;

                Some((option, value))
            }
            | "--enable" => {
                let option = param' |> List.hd;
                Some((option, "true"))
            } 
            | "--disable" => {
                let option = param' |> List.hd;
                Some((option, "false"))
            }
            | _ => None
        }
    })
    |> List.filter(e => e != None)
    |> List.map(e => e |> Option.get)
    |> List.filter(((k,_)) => !(String.starts_with(~prefix="server.", k))) // Don't allow server modifications
};