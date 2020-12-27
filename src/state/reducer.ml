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
  | ( `InspectFile _ | `InspectCilLocation _ | `UpdateCode _ | `InspectFunc _
    | `UpdateDot _ | `ResetInspect ) as a ->
      InspectReducer.reducer s a
  | `Switch_sidebar selected_sidebar -> { s with selected_sidebar }
  | `Switch_panel selected_panel -> { s with selected_panel }
  | (`UpdateSearchQuery _ | `ExecuteSearchQuery | `ClearSearchMatches) as a ->
      {
        s with
        syntactic_search = SyntacticSearchReducer.reducer s s.syntactic_search a;
      }
