module I = State.Inspect

let reducer (s : State.t) = function
  | `InspectFile (name, path) ->
      let inspect = Some (I.File { name; path; code = None }) in
      { s with file_name = name; file_path = path; inspect }
  | `InspectCilLocation l -> (
      match I.from_cil_location l s.pdata with
      | Some (File f) as inspect ->
          { s with file_name = f.name; file_path = f.path; inspect }
      | _ -> s )
  | `UpdateCode code -> (
      match s.inspect with
      | Some (File f) ->
          let inspect = Some (I.File { f with code = Some code }) in
          { s with code; inspect }
      | _ -> s )
  | `InspectGraph (name, file_name, file_path) ->
      let inspect = Some (I.Graph { name; file_name; file_path; dot = None }) in
      { s with inspect }
  | `UpdateDot dot -> (
      match s.inspect with
      | Some (Graph f) ->
          let inspect = Some (I.Graph { f with dot = Some dot }) in
          { s with inspect }
      | _ -> s )
  | `ResetInspect -> { s with inspect = None }
  | _ -> s
