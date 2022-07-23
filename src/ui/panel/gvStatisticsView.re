open Batteries;
module Stats = GoblintCil.Stats;

let rec make_task_list = tasks =>
  if (List.is_empty(tasks)) {
    React.null;
  } else {
    <ul>
      {tasks
       |> List.mapi((i, task: Stats.t) => {
            let key = string_of_int(i);
            <li key>
              {task.name
               ++ ": "
               ++ string_of_float(task.time)
               ++ " s"
               |> React.string}
              {task.sub |> List.rev |> make_task_list}
            </li>;
          })
       |> React.list}
    </ul>;
  };

let as_megabytes = words => Printf.sprintf("%.2f MB", words /. 131072.0);

[@react.component]
let make = (~stats) => {
  let (time: Stats.t, gc: Gc.stat) = stats;
  let total =
    time.sub |> List.fold_left((acc, sub: Stats.t) => acc +. sub.time, 0.0);

  <div>
    <span>
      {"Total: " ++ string_of_float(total) ++ " s" |> React.string}
    </span>
    {time.sub |> List.rev |> make_task_list}
    <span>
      {Printf.sprintf(
         "Memory: total=%s, max=%s, minor=%s, major=%s, promoted=%s, minor collections=%d, major collections=%d, compactions=%d",
         as_megabytes(gc.minor_words +. gc.major_words -. gc.promoted_words),
         as_megabytes(float_of_int(gc.top_heap_words)),
         as_megabytes(gc.minor_words),
         as_megabytes(gc.major_words),
         as_megabytes(gc.promoted_words),
         gc.minor_collections,
         gc.major_collections,
         gc.compactions,
       )
       |> React.string}
    </span>
  </div>;
};
