open Batteries
open GoblintCil
module D = GvDisplay

let reduce (s : State.t) (d : D.t option) = function
  | `DisplayNothing -> None
  | `DisplayFile path -> Some (D.file ~path ())
  | `SwitchToCil (file : D.file) ->
      if(Option.is_none file.cil) then (
        let global_to_string g = Pretty.sprint ~width:100 (printGlobal defaultCilPrinter () g) in
        let cil = foldGlobals s.cil (fun acc g ->
          if(String.equal file.path (get_globalLoc g).file) then
            acc ^ global_to_string g
          else
            acc) "" in
        Some (D.File {file with D.File.mode = CIL; D.File.cil = Some(cil)})
      ) else (
        Some (D.File {file with D.File.mode = CIL})
      )
  | `SwitchToC file -> Some (D.File {file with D.File.mode = C})
  | `DisplayFunc (name, file) ->
      let f = D.Func.make ~name ~file () in
      let fd = D.Func.fundec f s.cil in
      let dot = Option.map s.goblint#dot_of_fundec fd in
      Some (D.Func { f with dot })
  | `UpdateFileContents s -> (
      match d with Some (D.File f) -> Some (D.File { f with contents = Some s }) | _ -> d )
  | `UpdateFuncDot s -> (
      match d with Some (D.Func f) -> Some (D.Func { f with dot = Some s }) | _ -> d )
