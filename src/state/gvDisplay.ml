module File = struct
  type t = { path : string; contents : string option }

  let create ~path ?contents () = { path; contents }
end

module Func = struct
  type t = { name : string; file : string; dot : string option }

  let create ~name ~file ?dot () = { name; file; dot }
end

type file = File.t

type func = Func.t

type t = File of file | Func of func

let file ~path ?contents () = File (File.create ~path ?contents ())

let func ~name ~file ?dot () = Func (Func.create ~name ~file ?dot ())
