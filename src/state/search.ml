open Batteries

module Query = struct
  type t = ExpressionEvaluation.query

  type error = ParseError of string

  let create ?(kind = CodeQuery.Var_k) ?(target = CodeQuery.Name_t "") ?(find = CodeQuery.Uses_f)
      ?(structure = CodeQuery.None_s) ?(limitation = CodeQuery.None_c) ?(expression = "")
      ?(mode = `Must) ?(join = false) () : t =
          { kind; target; find; structure; limitation; expression; mode; join }

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

  let is_semantic (q : t) = not (String.is_empty q.expression)

  let execute (q : t) cil =
    if is_semantic q then (
      ExpressionEvaluation.gv_query := Some q;
      Maingoblint.do_analyze (Analyses.empty_increment_data cil) cil;
      let results = !ExpressionEvaluation.gv_results in
      let pred =
        if q.mode = `Must then snd %> fst %> Option.default false else snd %> fst %> Option.default true
      in
      let construct_element e =
        let (v, (_, c)) = e in
        (v, c)
      in
      results |> List.filter pred |> List.map construct_element)
    else 
      let result = QueryMapping.map_query (to_syntactic_query q) cil in
      List.map (fun e -> (e, None)) result

  let string_of_error e = match e with ParseError s -> s
end

type query = Query.t

module GraphicalUi = struct
  type target_error = ID_t of string

  type t = {
    kind : CodeQuery.kind;
    target : (CodeQuery.target, target_error) result;
    find : CodeQuery.find;
    structure : CodeQuery.structure;
    expression : string;
    mode : [ `Must | `May ];
    join: bool;
  }

  let create ?(kind = Query.default.kind) ?(target = Ok Query.default.target)
      ?(find = Query.default.find) ?(structure = Query.default.structure)
      ?(expression = Query.default.expression) ?(mode = Query.default.mode)
      ?(join=Query.default.join) () =
          { kind; target; find; structure; expression; mode; join }

  let default = create ()

  let to_query { kind; target; find; structure; expression; mode; join } =
    let target = Result.to_option target in
    Query.create ~kind ?target ~find ~structure ~expression ~mode ~join ()
end

type graphical_ui = GraphicalUi.t

type json_ui = { text : string; query : query option * Query.error option }

let graphical_ui_of_json_ui ju =
  match fst ju.query with
  | Some { kind; target; find; structure; expression; mode; join; _ } ->
      GraphicalUi.create ~kind ~target:(Ok target) ~find ~structure ~expression ~mode ~join ()
  | _ -> GraphicalUi.default

let json_ui_of_graphical_ui gu =
  let q = GraphicalUi.to_query gu in
  { text = Query.to_string q; query = (Some q, None) }

type mode = Graphical | Json

module Matches = struct
  type t = None | Loading | Done of ((string * Cil.location * string * int) * Printable.json option) list
end

type matches = Matches.t

type t = { mode : mode; graphical_ui : graphical_ui; json_ui : json_ui; matches : matches }

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
  let f q = { s with matches = Done (Query.execute q cil) } in
  Option.map_default f s q
