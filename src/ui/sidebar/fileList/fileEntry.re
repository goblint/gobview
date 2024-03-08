open React.Dom.Dsl;
open Html;
open Batteries;

let make_func_list = (file, funcs, dispatch) => {
  let on_click = (func, file, _) => dispatch @@ `DisplayFunc((func, file));

  funcs
  |> List.map(func => {
       <li key={file ++ func} className="list-group-item">
         <Link
           on_click={on_click(func, file)}
           class_=["text-link"]>
           ...{func |> React.string}
         </Link>
       </li>
     })
};
[@react.component]
let make = (~path, ~name, ~dispatch, ~functions, ~collapsed) => {
  let on_click = (file, _) => dispatch @@ `DisplayFile(file);
  let make_title = name =>
    <Link on_click={on_click(path)} class_=["text-link"]>
      ...{name |> React.string}
    </Link>;

  <CollapsibleList collapsed=collapsed>
    <CollapsibleListItem name make_title>
      <ul className="list-group">
        ...{make_func_list(path, functions, dispatch)}
      </ul>
    </CollapsibleListItem>
  </CollapsibleList>;
};
