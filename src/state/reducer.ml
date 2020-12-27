module I = State.Inspect

let reducer (s : State.t) a =
  match a with
  | `Set_id id -> { s with id }
  | `Set_line line -> { s with line }
  | `Set_file_name file_name -> { s with file_name }
  | `Set_file_path file_path -> { s with file_path }
  | `Set_cil cil -> { s with cil = Some cil }
  | `Set_pdata pdata -> { s with pdata }
  | `Set_code code -> { s with code }
  | `Inspect_file (I.File.Direct_location (n, p)) ->
      let inspect = Some (I.open_file_direct n p) in
      { s with file_name = n; file_path = p; inspect }
  | `Inspect_file (I.File.Cil_location l) ->
      let inspect = I.open_file_cil l s.pdata in
      let file_name, file_path =
        inspect
        |> Option.map (function
             | I.File f -> (I.File.get_name f, I.File.get_path f)
             | _ -> ("", ""))
        |> Option.value ~default:("", "")
      in
      { s with file_name; file_path; inspect }
  | `Update_code code -> (
      match s.inspect with
      | Some (File f) ->
          let inspect = Some (I.File { f with code = Some code }) in
          { s with code; inspect }
      | _ -> s )
  | `Inspect_function (name, file_name, file_path) ->
      let inspect = Some (I.Func { name; file_name; file_path; dot = None }) in
      { s with inspect }
  | `Update_dot dot -> (
      match s.inspect with
      | Some (Func f) ->
          let inspect = Some (I.Func { f with dot = Some dot }) in
          { s with inspect }
      | _ -> s )
  | `Reset_inspect -> { s with inspect = None }
  | `Switch_sidebar selected_sidebar -> { s with selected_sidebar }
  | `Switch_panel selected_panel -> { s with selected_panel }
  | `Update_query q ->
      let ss = s.syntactic_search in
      { s with syntactic_search = Syntactic_search_state.update_query ss q }
  | `Execute_query -> (
      let ss = s.syntactic_search in
      match s.cil with
      | Some c ->
          {
            s with
            syntactic_search = Syntactic_search_state.execute_query ss c;
          }
      | _ -> s )
  | `Clear_matches ->
      let ss = s.syntactic_search in
      { s with syntactic_search = Syntactic_search_state.clear_matches ss }
