open Batteries;

[@react.component]
let make = (~options, ~compare=?, ~value, ~on_change) => {
  let compare = Utils.fix_opt_arg(compare) |> Option.default((==));
  let options = options |> List.mapi((i, e) => (i, e));

  let i =
    options
    |> List.find_opt(((_, (v, _))) => compare(v, value))
    |> Option.map(fst %> string_of_int);

  let onChange = ev => {
    React.Event.Synthetic.preventDefault(ev);
    let i =
      React.Event.Synthetic.target(ev)
      |> Ojs.get_prop_ascii(_, "value")
      |> Ojs.string_of_js
      |> int_of_string;
    options |> List.assoc_opt(i) |> Option.map(fst) |> Option.may(on_change);
  };

  <select className="form-select" value=?i onChange>
    {options
     |> List.map(((i, (_, l))) => {
          let key = string_of_int(i);
          <option key value=key> {l |> React.string} </option>;
        })
     |> React.list}
  </select>;
};
