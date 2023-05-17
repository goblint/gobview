open Unix

let timeToString = (time) => {
    let rawMinutes = string_of_int(time.tm_min);
    let minutes = if (String.length(rawMinutes) == 1) {
        "0"
    } else {
        ""
    } ++ rawMinutes;

    minutes
    |> String.cat(":")
    |> String.cat(string_of_int(time.tm_hour))
    |> String.cat(" ");
};

let getLocalTime = () => {
    Unix.time()
    |> Unix.localtime
    |> timeToString;
};