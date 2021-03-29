let reducer (s : State.t) = function
  | `Set_id id -> { s with id }
  | `Set_line line -> { s with line }
  | `Set_file_name file_name -> { s with file_name }
  | `Set_file_path file_path -> { s with file_path }
  | `Set_cil cil -> { s with cil = Some cil }
  | `Set_pdata pdata -> { s with pdata }
  | `Set_code code -> { s with code }
  | `SwitchSidebar selected_sidebar -> { s with selected_sidebar }
  | `Switch_panel selected_panel -> { s with selected_panel }
  | ( `InspectFile _ | `InspectCilLocation _ | `UpdateCode _ | `InspectGraph _ | `UpdateDot _
    | `ResetInspect ) as a ->
      InspectReducer.reducer s a
  | ( `UpdateSearchMode _ | `UpdateSearchField _ | `ParseSearchQuery _ | `StartPerformSearch
    | `PerformSearch | `ClearSearchMatches ) as a ->
      { s with search = SearchReducer.reducer s.search (Option.get s.cil) a }
