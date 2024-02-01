open Batteries
module D = GvDisplay

let reduce (s : State.t) (d : D.t option) = function
  | `DisplayNothing -> None
  | `DisplayFile path -> Some (D.file ~path ())
  | `DisplayFunc (name, file) ->
      let f = D.Func.make ~name ~file () in
      let fd = D.Func.fundec f s.cil in
      let dot = Option.map s.goblint#dot_of_fundec fd in
      Some (D.Func { f with dot })
  | `DisplayUsageGraph ctx -> Some(D.Graph(ctx,s.display))
  | `UpdateFileContents s -> (
      match d with Some (D.File f) -> Some (D.File { f with contents = Some s }) | _ -> d )
  | `UpdateFuncDot s -> (
      match d with Some (D.Func f) -> Some (D.Func { f with dot = Some s }) | _ -> d )
