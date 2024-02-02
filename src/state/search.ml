open Goblint_lib
open Batteries
open GoblintCil
open Syntacticsearch

module Query = struct
  type t = ExpressionEvaluation.query
  type error = ParseError of string

  let create ?(kind = CodeQuery.Var_k) ?(target = CodeQuery.Name_t "") ?(find = CodeQuery.Uses_f)
      ?(structure = CodeQuery.None_s) ?(limitation = CodeQuery.None_c) ?(expression = None) (* TODO: limitation argument never used *)
      ?(mode = `Must) () : t =
    { kind; target; find; structure; limitation; expression; mode }

  let default = create ()

  let from_string s =
    try
      let json = Yojson.Safe.from_string s in
      match ExpressionEvaluation.query_of_yojson json with
      | Ppx_deriving_yojson_runtime.Result.Ok q -> Ok q
      | Ppx_deriving_yojson_runtime.Result.Error e -> Error (ParseError e)
    with Yojson.Json_error e -> Error (ParseError e)

  let to_string = ExpressionEvaluation.query_to_yojson %> Yojson.Safe.to_string

  let to_syntactic_query (q : t) : CodeQuery.query =
    { sel = []; k = q.kind; tar = q.target; f = q.find; str = q.structure; lim = q.limitation }

  let is_semantic (q : t) = not (Option.is_none q.expression)

  (* throws a QueryMapping.Not_supported exception if query is not supported *)
  let execute (q : t) cil =
    if is_semantic q then (
      ExpressionEvaluation.gv_query := Some q;
      Maingoblint.do_analyze None cil;
      let results = !ExpressionEvaluation.gv_results in
      let pred =
        if q.mode = `Must then snd %> Option.default false else snd %> Option.default true
      in
      results |> List.filter pred |> List.map fst)
    else QueryMapping.map_query (to_syntactic_query q) cil

  let string_of_error e = match e with ParseError s -> s
end

module GraphicalUi = struct
  type target_error = ID_t of string

  type t = {
    kind : CodeQuery.kind;
    target : (CodeQuery.target, target_error) result;
    find : CodeQuery.find;
    structure : CodeQuery.structure;
    expression : string option;
    mode : [ `Must | `May ];
  }

  let create ?(kind = Query.default.kind) ?(target = Ok Query.default.target)
      ?(find = Query.default.find) ?(structure = Query.default.structure)
      ?(expression = Query.default.expression) ?(mode = Query.default.mode) () =
    { kind; target; find; structure; expression; mode }

  let default = create ()

  let to_query { kind; target; find; structure; expression; mode } =
    let target = Result.to_option target in
    Query.create ~kind ?target ~find ~structure ~expression ~mode ()

end

type json_ui = { text : string; query : Query.t option * Query.error option }

let graphical_ui_of_json_ui ju =
  match fst ju.query with
  | Some { kind; target; find; structure; expression; mode; _ } ->
      GraphicalUi.create ~kind ~target:(Ok target) ~find ~structure ~expression ~mode ()
  | _ -> GraphicalUi.default

let json_ui_of_graphical_ui gu =
  let q = GraphicalUi.to_query gu in
  { text = Query.to_string q; query = (Some q, None) }

type mode = Graphical | Json

module Matches = struct
  type t = None | Loading | Done of (string * Cil.location * string * int) list | NotSupported
end

type matches = Matches.t

type t = { mode : mode; graphical_ui : GraphicalUi.t; json_ui : json_ui; matches : matches }

let default =
  {
    mode = Graphical;
    graphical_ui = GraphicalUi.default;
    json_ui = { text = ""; query = (None, None) };
    matches = None;
  }

let update_mode s mode =
  match (s.mode, mode) with
  | Graphical, Json -> { s with mode; json_ui = json_ui_of_graphical_ui s.graphical_ui }
  | Json, Graphical -> { s with mode; graphical_ui = graphical_ui_of_json_ui s.json_ui }
  | _ -> s

let execute s cil =
  let q =
    match s.mode with
    | Graphical -> Some (GraphicalUi.to_query s.graphical_ui)
    | Json -> fst s.json_ui.query
  in

  let f q =
    let r = try
      Matches.Done (Query.execute q cil)
    with QueryMapping.Not_supported -> Matches.NotSupported in
    { s with matches = r } in
  Option.map_default f s q
