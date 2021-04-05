open Batteries;

let make_func_list = (_, funcs) => {
  <ul>
    {funcs
     |> List.mapi((i, func) => {
          <li key={string_of_int(i)}> {func |> React.string} </li>
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
  <ul>
    {files
     |> Hashtbl.keys
     |> Enum.uniq_by(String.equal)
     |> Enum.mapi((i, file) => {
          <li key={string_of_int(i)}>
            {file |> React.string}
            {make_func_list(file, Hashtbl.find_all(files, file))}
          </li>
        })
     |> List.of_enum
     |> React.list}
  </ul>;
};
