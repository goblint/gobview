module S = State.SyntacticSearch

let reducer (s : State.t) a =
  let ss = s.syntactic_search in
  let syntactic_search =
    match a with
    | `ShowSynSearchQueryAsJSON show_json -> (
        let ss = { ss with show_json } in
        if show_json then
          (* Switching from UI mode to JSON mode *)
          let q = S.to_query ss in
          let query_json =
            q |> CodeQuery.query_to_yojson |> Yojson.Safe.to_string
          in
          { ss with query = Some q; query_json; query_json_error = None }
        else
          (* Switching from JSON mode to UI mode *)
          match ss.query with
          | Some q ->
              {
                ss with
                kind = q.k;
                target = Ok q.tar;
                find = q.f;
                structure = q.str;
              }
          | _ -> ss )
    | `UpdateSynSearchField f -> (
        match f with
        | `Kind kind -> { ss with kind }
        | `Target target -> { ss with target }
        | `Find find -> { ss with find }
        | `Structure structure -> { ss with structure } )
    | `ParseSynSearchQuery s ->
        let ss = { ss with query_json = s } in
        if String.length s > 0 then
          match S.Query.from_string s with
          | Ok q -> { ss with query = Some q; query_json_error = None }
          | Error e -> { ss with query = None; query_json_error = Some e }
        else { ss with query = None; query_json_error = None }
    | `PerformSynSearch -> (
        if ss.show_json then
          match (s.cil, ss.query) with
          | Some c, Some q ->
              let matches = Some (QueryMapping.map_query q c) in
              { ss with matches }
          | _ -> ss
        else
          match s.cil with
          | Some c ->
              let matches = Some (QueryMapping.map_query (S.to_query ss) c) in
              { ss with matches }
          | _ -> ss )
    | `ClearSynSearchMatches -> { ss with matches = None }
  in
  { s with syntactic_search }
