open Batteries;
open GoblintCil;
open FileTree;
open React.Dom.Dsl;
open Html;

let rec make_entries = (prefix, files, tree, mainFiles, dispatch) => {
  let prefix_of_main_file = (path) =>
    !List.exists((mf) => String.starts_with(mf, path), mainFiles);

  tree
  |> List.map(
       fun
       | FileTree.Directory(name, children) => {
           let path = prefix ++ name;
           <DirEntry name key=path collapsed=prefix_of_main_file(path)>
              ...{make_entries(path ++ "/", files, children, mainFiles, dispatch)}
           </DirEntry>;
         }
       | FileTree.File(name) => {
           let path = prefix ++ name;
           <FileEntry
             key=path
             functions={Hashtbl.find_all(files, path)}
             collapsed=prefix_of_main_file(path)
             path
             name
             dispatch
           />;
         },
     )
};

[@react.component]
let make = (~cil: Cil.file, ~dispatch) => {
  let files = Hashtbl.create(64);
  let (mf,_,_) = Goblint_lib.Cilfacade.getFuns(cil);
  let mainFiles = Cil.foldGlobals(
    cil,
    (mainFiles, g) =>
    switch(g) {
    | GFun(fdec, loc) => Hashtbl.add(files, loc.file, fdec.svar.vname);
      if(List.mem(fdec, mf)) {
        [loc.file, ...mainFiles]
      } else {
        mainFiles
      };
    | _ => mainFiles
    },
    [],
  );

  let tree = files |> Hashtbl.keys |> List.of_enum |> FileTree.mk_tree;

  <div>
  ...{make_entries("", files, tree, mainFiles, dispatch)}
  </div>
};
