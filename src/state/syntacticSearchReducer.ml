module S = State.Syntactic_search

let reducer (g : State.t) (s : S.t) = function
  | `Update_query q ->
      let s = { s with query_string = q } in
      if String.length q > 0 then
        match S.Query.parse_string q with
        | Ok q -> { s with query = Some q; query_error = None }
        | Error e -> { s with query = None; query_error = Some e }
      else { s with query = None; query_error = None }
  | `Execute_query -> (
      match (g.cil, s.query) with
      | Some c, Some q ->
          let matches = Some (QueryMapping.map_query q c) in
          { s with matches }
      | _ -> s )
  | `Clear_matches -> { s with matches = None }
