open React.Dom.Dsl.Html;

[@react.component]
let make = (~value, ~on_change) => {
  let value = value |> List.mapi((i, e) => (i, e));

  let (new_elem, set_new_elem) = React.use_state(() => "");

  let on_remove = (i, _) => {
    let value' = value |> List.remove_assoc(i) |> List.map(((_, e)) => e);
    on_change(value');
  };

  let on_type = v => {
    set_new_elem(_ => v);
  };

  let on_add = _ => {
    let value' = [new_elem, ...List.map(((_, e)) => e, value)];
    set_new_elem(_ => "");
    on_change(value');
  };

  <ul className="list-group">
    ...{List.map(((i, e)) => {
          <li
            key={string_of_int(i)}
            className="list-group-item d-flex justify-content-between align-items-center">
            {e |> React.string}
            <button
              type_="button"
              className="btn-close"
              onClick={on_remove(i)}
            />
          </li>
        }, value)
     @
    [<li className="list-group-item p-0">
      <div className="input-group">
        <Input value=new_elem on_change=on_type on_submit=on_add />
        <button
          type_="button" className="btn btn-outline-secondary" onClick=on_add>
          {"Add" |> React.string}
        </button>
      </div>
    </li>]
}
  </ul>;
};
