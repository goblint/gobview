open Batteries;

let make_func_list = (file, funcs, dispatch) => {
  let on_click = (data, _) => Option.may(dispatch, data);

  funcs
  |> List.map(func => {
       <li key={file ++ func} className="list-group-item px-5">
         <Link
           on_click
           callback_data={`DisplayFunc((func, file))}
           class_=["text-link"]>
           {func |> React.string}
         </Link>
       </li>
     })
  |> React.list;
};
[@react.component]
let make = (~path, ~name, ~dispatch, ~functions) => {
  let on_click = (data, _) => Option.may(dispatch, data);
  let make_title = name =>
    <Link on_click class_=["text-link"] callback_data={`DisplayFile(path)}>
      {name |> React.string}
    </Link>;

  <CollapsibleList>
    <CollapsibleListItem name make_title>
      <ul className="list-group">
        {make_func_list(path, functions, dispatch)}
      </ul>
    </CollapsibleListItem>
  </CollapsibleList>;
};
