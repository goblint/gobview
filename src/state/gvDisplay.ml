open Batteries
open GoblintCil

module File = struct
  type t = { path : string; contents : string option }

  let make ~path ?contents () = { path; contents }
end

module Func = struct
  type t = { name : string; file : string; dot : string option }

  let make ~name ~file ?dot () = { name; file; dot }

  let fundec f (cil : Cil.file) =
    cil.globals
    |> List.find_map_opt (function
         | Cil.GFun (fd, loc) ->
             if String.equal f.name fd.svar.vname && String.equal f.file loc.file then Some fd
             else None
         | _ -> None)
end

type file = File.t

type func = Func.t

type t = File of file | Func of func | Graph of (Representation.t option * t option)

let file ~path ?contents () = File (File.make ~path ?contents ()) (* TODO: contents argument never used *)

(* TODO: unused *)
let func ~name ~file ?dot () = Func (Func.make ~name ~file ?dot ())
