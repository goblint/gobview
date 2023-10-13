open Batteries

module FileTree = struct
  (*
    Modified trie / prefix tree
    Files are Leafs.
    The whole filepath can be reconstructed by concatinating the prefixes of its parents.
    Directory("test",[File("a.c")]) => "test/a.c"
  *)
  type t = File of string
    | Directory of string * t list

  (*
    Compact a tree by joining Directories with only a single child.
    Directory("src",[Directory("test",[...])])=> Directory("src/test",[...])
  *)
  let rec compact = function
    | Directory(p, [Directory(pp, r)]) ->
      Directory(p ^ "/" ^ pp, r) |> compact
    | t -> t

  let rec mk_tree_seq_lists (paths : string list list) =
    paths
    |> List.group (fun ls1 ls2 -> compare (List.hd ls1) (List.hd ls2))
    |> List.map
         (function
         (* Unique prefix => no second entry with this path. Therefore has to be a single file *)
         | [file_path] -> File(String.concat "/" file_path)
         | group_paths -> (
             (* All elements in this group will have the same prefix by definition *)
             let prefix =
               group_paths |> List.hd |> List.hd in
             let tree = group_paths
              |> List.map List.tl
              |> mk_tree_seq_lists in
             Directory (prefix, tree)
             |> compact;
         )
       )

  let mk_tree = fun (paths : string list) ->
    paths
    |> List.unique
    |> List.map Fpath.v
    |> List.map Fpath.segs
    |> mk_tree_seq_lists
end
