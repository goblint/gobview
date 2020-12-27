module S = State.SyntacticSearch

let reducer (s : State.t) a =
  let ss = s.syntactic_search in
  let syntactic_search =
    match a with
    | `UpdateSearchQuery q ->
        let ss = { ss with query_string = q } in
        if String.length q > 0 then
          match S.Query.from_string q with
          | Ok q -> { ss with query = Some q; query_error = None }
          | Error e -> { ss with query = None; query_error = Some e }
        else { ss with query = None; query_error = None }
    | `ExecuteSearchQuery -> (
        match (s.cil, ss.query) with
        | Some c, Some q ->
            let matches = Some (QueryMapping.map_query q c) in
            { ss with matches }
        | _ -> ss )
    | `ClearSearchMatches -> { ss with matches = None }
    | _ -> ss
  in
  { s with syntactic_search }
