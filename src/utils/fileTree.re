open Batteries;

module FileTree = {
  /*
    Modified trie / prefix tree
    Files are Leafs.
    The whole filepath can be reconstructed by concatinating the prefixes of its parents.
    Directory("test",[File("a.c")]) => "test/a.c"
  */
  type t =
    | File(string)
    | Directory(string, list(t));

  let split_first_directory = path =>
    if (String.contains_from(path, 1, '/')) {
      // Prevent splits on the first character (ex. "/home" shouldn't split)
      String.split(path |> String.lchop, ~by="/")
      |> Tuple2.map1(s => String.head(path, 1) ++ s);
    } else {
      (path, "");
    };

  /*
    Compare 2 arguments according to their first directory
    ("/home/a","/home/b") => 0 (equal)
    ("/home/a","/usr/a") != 0
  */
  let compare_prefix =
    curry(Tuple2.mapn(split_first_directory %> fst) %> uncurry(compare));

  /*
    Compact a tree by joining Directories with only a single child.
    Directory("src",[Directory("test",[...])])=> Directory("src/test",[...])
  */
  let rec compact =
    fun
    | Directory(p, [Directory(pp, r)]) =>
      Directory(p ++ "/" ++ pp, r) |> compact
    | t => t;
  let rec mk_tree = paths => {
    paths
    |> List.unique
    |> List.group(compare_prefix)
    |> List.map(
         fun
         // Unique prefix => no second entry with this path. Therefore has to be a single file
         | [file_path] => File(file_path)
         | group_paths => {
             // All elements in this group will have the same prefix by definition
             let prefix =
               group_paths |> List.hd |> split_first_directory |> fst;

             Directory(
               prefix,
               group_paths
               |> List.map(split_first_directory %> snd)
               |> List.remove_all(_, "")
               |> mk_tree,
             )
             |> compact;
           },
       );
  };
};
