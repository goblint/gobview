open Batteries;

let rec make_task_list = tasks =>
  if (List.is_empty(tasks)) {
    React.null;
  } else {
    <ul>
      {tasks
       |> List.mapi((i, task: Timing.tree) => {
            let key = string_of_int(i);
            <li key>
              {task.name
               ++ ": "
               ++ string_of_float(task.cputime)
               ++ " s"
               |> React.string}
              {task.children |> List.rev |> make_task_list}
            </li>;
          })
       |> React.list}
    </ul>;
  };

let as_megabytes = words => Printf.sprintf("%.2f MB", words /. 131072.0);

[@react.component]
let make = (~stats) => {
  let (time: Timing.tree, gc: Gc.stat) = stats;
  let total =
    time.children |> List.fold_left((acc, sub: Timing.tree) => acc +. sub.cputime, 0.0);

  <div>
    {if(total == 0.) {
      <div className="bg-warning">
        {"For timing statistics the analysis has to be run with -v (alternatively, either option dbg.verbose or dbg.timing.enabled can be activated)" |> React.string}
      </div>;
    } else {
      React.null
    }}
    <div>
      {"Total: " ++ string_of_float(total) ++ " s" |> React.string}
    </div>
    {time.children |> List.rev |> make_task_list}
    <div>
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
    </div>
  </div>;
};
