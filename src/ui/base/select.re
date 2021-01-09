let value_by_index = (i, options) =>
  List.assoc_opt(i, options) |> Option.map(((v, _)) => v);

let index_by_value = v =>
  List.find_map(((i, (v', _))) =>
    if (v' == v) {
      Some(i);
    } else {
      None;
    }
  );

[@react.component]
let make = (~options, ~value, ~on_change) => {
  let options = options |> List.mapi((i, e) => (i, e));

  let i =
    index_by_value(value, options)
    |> Option.value(~default=0)
    |> string_of_int;

  let onChange = ev => {
    React.Event.Synthetic.preventDefault(ev);
    let i =
      React.Event.Synthetic.target(ev)
      |> Ojs.get(_, "value")
      |> Ojs.string_of_js
      |> int_of_string;
    Option.iter(on_change, value_by_index(i, options));
  };

  <select className="form-select" value=i onChange>
    {options
     |> List.map(((i, (_, l))) => {
          let key = string_of_int(i);
          <option key value=key> {l |> React.string} </option>;
        })
     |> React.list}
  </select>;
};
