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
 
let construct_parameters = (parameters) => {
    parameters
    |> String.split_on_char(' ')
    //|> List.map((s) => /*"'" |> String.cat(*/s/*) |> String.cat("'")*/)
}

// TODO combine --set and the following <option> to one string
// TODO afterwards, create a list with combined and following value, like so: ["--set <option>", value]
let rec tuples_from_parameters = (parameters) => {
    if (parameters |> List.length > 0) {
        
        let (command, tail) = (parameters |> List.hd, parameters |> List.tl);
        switch command {
            | "--set" => {
                let (option, tail') = (tail |> List.hd, tail |> List.tl);
                let (value, tail'') = (tail' |> List.hd, tail' |> List.tl);

                let command' = Printf.sprintf("--set %s", option);
                let tuple = (command', value);
                tail'' |> tuples_from_parameters |> List.cons(tuple)
            }
            | "--enable" => {
                let (option, tail') = (tail |> List.hd, tail |> List.tl);

                let tuple = (command, option);
                tail' |> tuples_from_parameters |> List.cons(tuple)
            } 
            | _ => []
        }
    } else {
        []
    }
}