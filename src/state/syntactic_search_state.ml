module Query = struct
  type error = Parse_error of string

  let string_of_error e = match e with Parse_error s -> s

  type t = CodeQuery.query

  type parse_result = (t, error) result

  let parse_string s =
    try
      let y = Yojson.Safe.from_string s in
      match CodeQuery.query_of_yojson y with
      | Ppx_deriving_yojson_runtime.Result.Ok q -> Ok q
      | Ppx_deriving_yojson_runtime.Result.Error e -> Error (Parse_error e)
    with Yojson.Json_error e -> Error (Parse_error e)

  type matches = (string * Cil.location * string * int) list
end

type query = Query.t

type t = {
  query_string : string;
  query : query option;
  query_error : Query.error option;
  matches : Query.matches option;
}

let default =
  { query_string = ""; query = None; query_error = None; matches = None }

let get_query_string s = s.query_string

let get_query s = s.query

let get_query_error s = s.query_error

let get_matches s = s.matches

let update_query s q =
  let s = { s with query_string = q } in
  if String.length q > 0 then
    match Query.parse_string q with
    | Ok q -> { s with query = Some q; query_error = None }
    | Error e -> { s with query = None; query_error = Some e }
  else { s with query = None; query_error = None }

let execute_query s cil =
  match s.query with
  | Some q ->
      let matches = Some (QueryMapping.map_query q cil) in
      { s with matches }
  | _ -> s

let clear_matches s = { s with matches = None }
