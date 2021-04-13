open Batteries;

let make_func_list = (file, funcs, dispatch) => {
  let on_click = (data, _) => Option.may(dispatch, data);

  <ul>
    {funcs
     |> List.mapi((i, func) => {
          <li key={string_of_int(i)}>
            <Link on_click callback_data={`DisplayFunc((func, file))}>
              {func |> React.string}
            </Link>
          </li>
        })
     |> React.list}
  </ul>;
};

[@react.component]
let make = (~cil: Cil.file, ~dispatch) => {
  let files = Hashtbl.create(64);
  Cil.iterGlobals(
    cil,
    fun
    | GFun(fdec, loc) => Hashtbl.add(files, loc.file, fdec.svar.vname)
    | _ => (),
  );

  let on_click = (data, _) => Option.may(dispatch, data);

  <ul>
    {files
     |> Hashtbl.keys
     |> Enum.uniq_by(String.equal)
     |> Enum.mapi((i, file) => {
          <li key={string_of_int(i)}>
            <Link on_click callback_data={`DisplayFile(file)}>
              {file |> React.string}
            </Link>
            {make_func_list(file, Hashtbl.find_all(files, file), dispatch)}
          </li>
        })
     |> List.of_enum
     |> React.list}
  </ul>;
};
