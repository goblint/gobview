let reducer (s : Search.t) cil = function
  | `UpdateSearchMode m -> Search.update_mode s m
  | `UpdateSearchField f ->
      let gu = s.graphical_ui in
      let graphical_ui =
        match f with
        | `Kind kind -> { gu with kind }
        | `Target target -> { gu with target }
        | `Find find -> { gu with find }
        | `Structure structure -> { gu with structure }
        | `Expression expression -> { gu with expression }
        | `Mode mode -> { gu with mode }
      in
      { s with graphical_ui }
  | `ParseSearchQuery text ->
      let ju = { s.json_ui with text } in
      let json_ui =
        if String.length text > 0 then
          match Search.Query.from_string text with
          | Ok q -> { ju with query = (Some q, None) }
          | Error e -> { ju with query = (None, Some e) }
        else { ju with query = (None, None) }
      in
      { s with json_ui }
  | `StartPerformSearch -> { s with matches = Loading }
  | `PerformSearch -> Search.execute s cil
  | `ClearSearchMatches -> { s with matches = None }
