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

// jsoo-react seems to have some trouble with
// non-mandatory component properties. This is a
// workaround until the issue is resolved.
let make_optionals = (~id=?, ~on_change=?, ()) => {
  (id, on_change);
};

[@react.component]
let make = (~options, ~value, ~optionals) => {
  let (id, on_change) = optionals;

  let options = options |> List.mapi((i, e) => (i, e));

  let i =
    index_by_value(value, options)
    |> Option.value(~default=0)
    |> string_of_int;

  let onChange = ev => {
    on_change
    |> Option.iter(cb => {
         let i =
           React.Event.Synthetic.target(ev)
           |> Ojs.get(_, "value")
           |> Ojs.int_of_js;
         value_by_index(i, options) |> Option.iter(cb);
       });
  };

  <select ?id className="form-select" value=i onChange>
    {options
     |> List.map(((i, (_, l))) => {
          let key = string_of_int(i);
          <option key value=key> {l |> React.string} </option>;
        })
     |> React.list}
  </select>;
};
