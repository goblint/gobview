open Batteries
open GoblintCil

type fmode = C | CIL

module File = struct
  type t = { path : string; contents : string option; cil : string option; mode : fmode }

  let make ~path ~mode ?contents ?cil () = { path; mode; contents; cil; }
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

type t = File of file | Func of func

let file ~path ?(mode=C) ?contents ?cil () = File (File.make ~path ~mode ?contents ?cil ()) (* TODO: contents argument never used *)

(* TODO: unused *)
let func ~name ~file ?dot () = Func (Func.make ~name ~file ?dot ())
