module Query = struct
  type t = CodeQuery.query

  type error = Parse_error of string

  let from_string s =
    try
      let y = Yojson.Safe.from_string s in
      match CodeQuery.query_of_yojson y with
      | Ppx_deriving_yojson_runtime.Result.Ok q -> Ok q
      | Ppx_deriving_yojson_runtime.Result.Error e -> Error (Parse_error e)
    with Yojson.Json_error e -> Error (Parse_error e)

  let string_of_error e = match e with Parse_error s -> s
end

type query = Query.t

type t = {
  query_string : string;
  query : query option;
  query_error : Query.error option;
  matches : (string * Cil.location * string * int) list option;
}

let default =
  { query_string = ""; query = None; query_error = None; matches = None }
