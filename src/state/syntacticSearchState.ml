module Query = struct
  type t = CodeQuery.query

  type error = Parse_error of string

  let from_string s =
    try
      let json = Yojson.Safe.from_string s in
      match CodeQuery.query_of_yojson json with
      | Ppx_deriving_yojson_runtime.Result.Ok q -> Ok q
      | Ppx_deriving_yojson_runtime.Result.Error e -> Error (Parse_error e)
    with Yojson.Json_error e -> Error (Parse_error e)

  let string_of_error e = match e with Parse_error s -> s
end

type query = Query.t

type t = {
  show_json : bool;
  (* UI mode *)
  kind : CodeQuery.kind;
  target : (CodeQuery.target, SyntacticSearchTargetBuilder.error) result;
  find : CodeQuery.find;
  structure : CodeQuery.structure;
  (* JSON mode *)
  query_json : string;
  query : query option;
  query_json_error : Query.error option;
  (* Search results *)
  matches : (string * Cil.location * string * int) list option;
}

let default =
  {
    show_json = false;
    kind = Var_k;
    target = Ok (Name_t "");
    find = Uses_f;
    structure = None_s;
    query_json = "";
    query = None;
    query_json_error = None;
    matches = None;
  }

let to_query s =
  {
    CodeQuery.sel = [];
    k = s.kind;
    tar = Result.value s.target ~default:(CodeQuery.Name_t "");
    f = s.find;
    str = s.structure;
    lim = None_c;
  }
