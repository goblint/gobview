open React.Dom.Dsl.Html;
open Batteries;
open GoblintCil;

let make_func_list = (file, funcs, dispatch) => {
  let on_click = (data, _) => Option.may(dispatch, data);

  {funcs
    |> List.mapi((i, func) => {
        <li key={"inner" ++ string_of_int(i)} className="list-group-item px-5">
          <Link on_click callback_data={`DisplayFunc((func, file))} class_=["text-link"]>
            ...{func |> React.string}
          </Link>
        </li>
      })
    |> React.list};
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

  <ul className="list-group">
    ...{files
     |> Hashtbl.keys
     |> Enum.uniq_by(String.equal)
     |> Enum.mapi((i, file) => {
          <div key={string_of_int(i)}>
            <li className="list-group-item">
              <Link on_click callback_data={`DisplayFile(file)} class_=["text-link"]>
                ...{file |> React.string}
              </Link>
            </li>
            {make_func_list(file, Hashtbl.find_all(files, file), dispatch)}
          </div>
        })
     |> List.of_enum}
  </ul>;
};
