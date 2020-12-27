let map = function
  | `InspectFile _ | `InspectCilLocation _ | `UpdateCode _ | `InspectGraph _
  | `UpdateDot _ | `ResetInspect ->
      InspectReducer.reducer
  | `UpdateSearchQuery _ | `ExecuteSearchQuery | `ClearSearchMatches ->
      SyntacticSearchReducer.reducer
  | _ -> failwith "Unrecognized reducer action"

let reducer (s : State.t) a =
  match a with
  | `Set_id id -> { s with id }
  | `Set_line line -> { s with line }
  | `Set_file_name file_name -> { s with file_name }
  | `Set_file_path file_path -> { s with file_path }
  | `Set_cil cil -> { s with cil = Some cil }
  | `Set_pdata pdata -> { s with pdata }
  | `Set_code code -> { s with code }
  | `Switch_sidebar selected_sidebar -> { s with selected_sidebar }
  | `Switch_panel selected_panel -> { s with selected_panel }
  | _ as a -> map a s a
