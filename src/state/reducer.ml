let reducer (s : State.t) = function
  | `Set_line line -> { s with line }
  | `Set_file_name file_name -> { s with file_name }
  | `Set_file_path file_path -> { s with file_path }
  | `SwitchSidebar selected_sidebar -> { s with selected_sidebar }
  | `Switch_panel selected_panel -> { s with selected_panel }
  | (`DisplayNothing | `DisplayFile _ | `DisplayFunc _ | `UpdateFileContents _ | `UpdateFuncDot _)
    as a ->
      { s with display = GvDisplayReducer.reduce s s.display a }
  | (`InspectLine _ | `InspectNode _) as a ->
      { s with inspect = GvInspectReducer.reduce s s.inspect a }
  | ( `UpdateSearchMode _ | `UpdateSearchField _ | `ParseSearchQuery _ | `StartPerformSearch
    | `PerformSearch | `ClearSearchMatches ) as a ->
      { s with search = SearchReducer.reducer s.search s.cil a }
