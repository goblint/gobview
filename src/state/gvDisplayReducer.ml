module D = GvDisplay

let reduce (_ : State.t) (d : D.t option) = function
  | `DisplayNothing -> None
  | `DisplayFile path -> Some (D.file ~path ())
  | `DisplayFunc (name, file) -> Some (D.func ~name ~file ())
  | `UpdateFileContents s -> (
      match d with Some (D.File f) -> Some (D.File { f with contents = Some s }) | _ -> d )
  | `UpdateFuncDot s -> (
      match d with Some (D.Func f) -> Some (D.Func { f with dot = Some s }) | _ -> d )
