open Unix

let time_to_string = (time) => {
    let rawMinutes = string_of_int(time.tm_min);

    rawMinutes
    |> String.cat(if (String.length(rawMinutes) == 1) { "0" } else { "" })
    |> String.cat(":")
    |> String.cat(string_of_int(time.tm_hour))
    |> String.cat(" ");
};

let get_local_time = () => {
    Unix.time()
    |> Unix.localtime
    |> time_to_string;
};