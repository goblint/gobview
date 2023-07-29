let reducer (s : State.t) = function
  | `SwitchSidebarLeft selected_sidebar_left -> { s with selected_sidebar_left }
  | `SwitchSidebarRight selected_sidebar_right -> { s with selected_sidebar_right }
  | `SwitchPanel selected_panel -> { s with selected_panel }
  | (`DisplayNothing | `DisplayFile _ | `DisplayFunc _ | `DisplayUsageGraph _  | `UpdateFileContents _ | `UpdateFuncDot _)
    as a ->
      { s with display = GvDisplayReducer.reduce s s.display a }
  | (`InspectLine _ | `InspectNode _) as a ->
      { s with inspect = GvInspectReducer.reduce s s.inspect a }
  | ( `UpdateSearchMode _ | `UpdateSearchField _ | `ParseSearchQuery _ | `StartPerformSearch
    | `PerformSearch | `ClearSearchMatches ) as a ->
      { s with search = SearchReducer.reducer s.search s.cil a }
