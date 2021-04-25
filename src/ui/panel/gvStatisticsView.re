open Batteries;

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

[@react.component]
let make = (~stats: Stats.t) => {
  let time =
    stats.sub |> List.fold_left((acc, sub: Stats.t) => acc +. sub.time, 0.0);

  <div>
    {"Total: " ++ string_of_float(time) ++ " s" |> React.string}
    {stats.sub |> List.rev |> make_task_list}
  </div>;
};
