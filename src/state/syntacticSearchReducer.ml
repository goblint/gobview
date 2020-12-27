module S = State.SyntacticSearch

let reducer (g : State.t) (s : S.t) = function
  | `UpdateSearchQuery q ->
      let s = { s with query_string = q } in
      if String.length q > 0 then
        match S.Query.from_string q with
        | Ok q -> { s with query = Some q; query_error = None }
        | Error e -> { s with query = None; query_error = Some e }
      else { s with query = None; query_error = None }
  | `ExecuteSearchQuery -> (
      match (g.cil, s.query) with
      | Some c, Some q ->
          let matches = Some (QueryMapping.map_query q c) in
          { s with matches }
      | _ -> s )
  | `ClearSearchMatches -> { s with matches = None }
