module File = struct
  type t = { path : string; contents : string option }

  let make ~path ?contents () = { path; contents }
end

module Func = struct
  type t = { name : string; file : string; dot : string option }

  let make ~name ~file ?dot () = { name; file; dot }
end

type file = File.t

type func = Func.t

type t = File of file | Func of func

let file ~path ?contents () = File (File.make ~path ?contents ())

let func ~name ~file ?dot () = Func (Func.make ~name ~file ?dot ())
