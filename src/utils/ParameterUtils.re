open State

let concatParameters = (parameters) => {
    parameters
    |> String.split_on_char(' ')
    |> List.map((s) => { String.sub(s, 1, String.length(s)-2)})
    |> String.concat(" ")
};

let getParameters = (state) => {
    switch (Yojson.Safe.Util.member("command", state.meta)) {
        | `String(command) => command
        | _ => ""
    }
};
 
let constructParameters = (parameters) => {
    let param_list = parameters
                     |> String.split_on_char(' ')
                     |> List.map((s) => "'" |> String.cat(s) |> String.cat("'"));
    
    (param_list |> List.hd, param_list)
}