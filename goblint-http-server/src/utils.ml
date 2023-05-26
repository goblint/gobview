open Batteries

let temp_dir_name = Filename.get_temp_dir_name ()

let temp_dir prefix suffix =
  let rec loop count =
    if count <= 0 then
      failwith "Cannot create temporary directory";
    let random = Random.int 100_000 in
    let name = Printf.sprintf "%s%d%s" prefix random suffix in
    let path = Filename.concat temp_dir_name name in
    try
      Sys.mkdir path 0o700;
      path
    with _ -> loop (count - 1)
  in loop 100
