open State

type action =
  | Set_id of int
  | Set_line of int
  | Set_file_name of string
  | Set_file_path of string
  | Set_cil of Cil.file
  | Set_pdata of Parse.run
  | Set_code of string
  | Inspect_file of Inspect.File.location
  | Update_code of string
  | Inspect_function of string * string * string
  | Update_dot of string
  | Reset_inspect
  | Switch_sidebar of selected_sidebar
  | Switch_panel of selected_panel option
  | Update_query of string
  | Execute_query
  | Clear_matches

let reducer (s : t) (a : action) =
  match a with
  | Set_id id -> { s with id }
  | Set_line line -> { s with line }
  | Set_file_name file_name -> { s with file_name }
  | Set_file_path file_path -> { s with file_path }
  | Set_cil cil -> { s with cil = Some cil }
  | Set_pdata pdata -> { s with pdata }
  | Set_code code -> { s with code }
  | Inspect_file (Direct_location (n, p)) ->
      let inspect = Some (Inspect.open_file_direct n p) in
      { s with file_name = n; file_path = p; inspect }
  | Inspect_file (Cil_location l) ->
      let inspect = Inspect.open_file_cil l s.pdata in
      let file_name, file_path =
        inspect
        |> Option.map (function
             | Inspect.File f ->
                 (Inspect.File.get_name f, Inspect.File.get_path f)
             | _ -> ("", ""))
        |> Option.value ~default:("", "")
      in
      { s with file_name; file_path; inspect }
  | Update_code code -> (
      match s.inspect with
      | Some (File f) ->
          let inspect = Some (Inspect.File { f with code = Some code }) in
          { s with code; inspect }
      | _ -> s )
  | Inspect_function (name, file_name, file_path) ->
      let inspect =
        Some (Inspect.Func { name; file_name; file_path; dot = None })
      in
      { s with inspect }
  | Update_dot dot -> (
      match s.inspect with
      | Some (Func f) ->
          let inspect = Some (Inspect.Func { f with dot = Some dot }) in
          { s with inspect }
      | _ -> s )
  | Reset_inspect -> { s with inspect = None }
  | Switch_sidebar selected_sidebar -> { s with selected_sidebar }
  | Switch_panel selected_panel -> { s with selected_panel }
  | Update_query q ->
      let ss = s.syntactic_search in
      { s with syntactic_search = Syntactic_search_state.update_query ss q }
  | Execute_query -> (
      let ss = s.syntactic_search in
      match s.cil with
      | Some c ->
          {
            s with
            syntactic_search = Syntactic_search_state.execute_query ss c;
          }
      | _ -> s )
  | Clear_matches ->
      let ss = s.syntactic_search in
      { s with syntactic_search = Syntactic_search_state.clear_matches ss }
