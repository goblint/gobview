open Batteries;
open GoblintCil;
open FileTree;

let rec make_entries = (prefix, files, tree, dispatch) => {
  tree
  |> List.map(
       fun
       | FileTree.Directory(name, children) => {
           let path = prefix ++ name;
           <DirEntry name key=path>
             {make_entries(path ++ "/", files, children, dispatch)}
           </DirEntry>;
         }
       | FileTree.File(name) => {
           let path = prefix ++ name;
           <FileEntry
             key=path
             functions={Hashtbl.find_all(files, path)}
             path
             name
             dispatch
           />;
         },
     )
  |> React.list;
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

  let tree = files |> Hashtbl.keys |> List.of_enum |> FileTree.mk_tree;

  make_entries("", files, tree, dispatch);
};
