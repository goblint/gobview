open State;

let concat_parameter_list = String.concat(" ");
let concat_grouped_parameters = (parameters) => parameters |> List.map(concat_parameter_list) |> concat_parameter_list;

let parameters_regex = Str.regexp({|\(--\(enable\|disable\) [-a-zA-Z0-9\._]+\)\|\(--set [-a-zA-Z0-9\._]+\(\[\(\+\|\-\)\]\)? \([-a-zA-Z0-9\._\/]+\|\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\|\[\(\(\(\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\|[-a-zA-Z0-9\._\/]+\)\(\([ ]*,[ ]*\)\(\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\|[-a-zA-Z0-9\._\/]+\)\)*\)\|\(\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\)\|[-a-zA-Z0-9\._\/]*\)\]\)\)\|\(--[-a-zA-Z0-9\._]+\(\[\(\+\|\-\)\]\)? \([-a-zA-Z0-9\._\/]+\|\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\|\[\(\(\(\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\|[-a-zA-Z0-9\._\/]+\)\(\([ ]*,[ ]*\)\(\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\|[-a-zA-Z0-9\._\/]+\)\)*\)\|\(\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\)\|[-a-zA-Z0-9\._\/]*\)\]\)\)|});
let enable_disable_regex = Str.regexp({|--\(enable\|disable\) \([-a-zA-Z0-9\._]+\)|});
let set_regex = Str.regexp({|--set \([-a-zA-Z0-9\._]+\(\[\(\+\|\-\)\]\)?\) \([-a-zA-Z0-9\._\/]+\|\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\|\[\(\(\(\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\|[-a-zA-Z0-9\._\/]+\)\(\([ ]*,[ ]*\)\(\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\|[-a-zA-Z0-9\._\/]+\)\)*\)\|\(\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\)\|[-a-zA-Z0-9\._\/]*\)\]\)|});
let other_regex = Str.regexp({|--\([-a-zA-Z0-9\._]+\(\[\(\+\|\-\)\]\)?\) \([-a-zA-Z0-9\._\/]+\|\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\|\[\(\(\(\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\|[-a-zA-Z0-9\._\/]+\)\(\([ ]*,[ ]*\)\(\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\|[-a-zA-Z0-9\._\/]+\)\)*\)\|\(\("\|'\)[-a-zA-Z0-9\._, \!\?\$\*\/]*\("\|'\)\)\|[-a-zA-Z0-9\._\/]*\)\]\)|});
let string_with_upticks_regex = Str.regexp({|'\(.*\)'|});

// Reads Goblint's path and the parameters which were taken into consideration by Goblint for the first analysis
let get_parameters = (state) => {
    let raw_parameters = switch (Yojson.Safe.Util.member("command", state.meta)) {
        | `String(command) => command
        | _ => ""
    }
    |> String.split_on_char(' ')
    |> List.map((s) => { String.sub(s, 1, String.length(s)-2) });

    let goblint_path = raw_parameters |> List.hd;
    let parameters = raw_parameters |> List.tl |> concat_parameter_list;

    (goblint_path, parameters);
};

// Reads and extracts every parameter and returns additionally whether the input is malformed or well formed
let construct_parameters = (parameters: string): (list(string), bool) => {
    let replaced_words = ref([]);
    let result = parameters |> Str.global_substitute(parameters_regex, (substring => {
        replaced_words := replaced_words.contents |> List.cons(Str.matched_group(0, substring));
        ""
    }));

    (replaced_words.contents, (result |> String.trim |> String.length) > 0)
};

let tuples_from_parameters = (grouped_parameters: list(string)): list((string,string)) => {
    grouped_parameters
    |> List.map((parameter) => {
        // The string has to be compared to a regex instead of using String.starts_with as Str.matched_group is used in each case below; otherwise an exc will be raised
        // Additionally, every Str.matched_group call has to be preceeded by an independent Str.string_match otherwise it takes the group or returns a wrong substring
        // This order of cases was chosen because "--" matches to all and should be the last option therefore
        if (Str.string_match(enable_disable_regex, parameter, 0)) {
            switch ((Str.matched_group(1, parameter), Str.matched_group(2, parameter))) {
                | ("enable", option) => Some((option, "true"))
                | ("disable", option) => Some((option, "false"))
                | exception Not_found => None
                | _ => None
            }
        } else if (Str.string_match(set_regex, parameter, 0) || Str.string_match(other_regex, parameter, 0)) {
            switch ((Str.matched_group(1, parameter), Str.matched_group(4, parameter))) {
                | (option, value) => Some((option, value))
                | exception Not_found => None
            }
        } else {
            None
        }
    })
    |> List.filter(e => !Option.is_none(e))
    |> List.map(e => {
        let (o,v) = (e |> Option.get);
        let v' = Str.global_replace(string_with_upticks_regex, {|"\1"|}, v);
        (o,v')
    })
};