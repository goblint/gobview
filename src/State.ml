module Inspect = struct
  module File = struct
    type t = { name : string; path : string; code : string option }

    let name f = f.name

    let path f = f.path

    let code f = Option.get f.code

    let code_opt f = f.code
  end

  module Func = struct
    type t = {
      name : string;
      file_name : string;
      file_path : string;
      dot : string option;
    }

    let name f = f.name

    let file_name f = f.file_name

    let file_path f = f.file_path

    let dot f = Option.get f.dot

    let dot_opt f = f.dot
  end

  type file = File.t

  type func = Func.t

  type t = File of file | Func of func
end

type inspect = Inspect.t

module Selected_sidebar = struct
  type t = State | Globals
end

type selected_sidebar = Selected_sidebar.t

module Selected_panel = struct
  type t = Warnings | Dead_code | Parameters | Statistics
end

type selected_panel = Selected_panel.t

module Syntactic_search = struct
  type error = Parse_error of string

  let string_of_error e = match e with Parse_error s -> s

  type t = {
    query_text : string;
    query : (CodeQuery.query, error) result option;
    matches : (string * Cil.location * string * int) list option;
  }

  let default = { query_text = ""; query = None; matches = None }

  let get_query_text ss = ss.query_text

  let get_query ss = ss.query

  let get_matches ss = ss.matches

  let try_to_parse json =
    try
      let parsed = Yojson.Safe.from_string json in
      match CodeQuery.query_of_yojson parsed with
      | Ppx_deriving_yojson_runtime.Result.Ok q -> Ok q
      | Ppx_deriving_yojson_runtime.Result.Error e -> Error (Parse_error e)
    with Yojson.Json_error e -> Error (Parse_error e)
end

type syntactic_search = Syntactic_search.t

type t = {
  id : int;
  line : int;
  file_name : string;
  file_path : string;
  cil : Cil.file option;
  pdata : Parse.run;
  code : string;
  inspect : inspect option;
  selected_view : SelectedView.t;
  selected_sidebar : selected_sidebar;
  selected_panel : selected_panel option;
  syntactic_search : syntactic_search;
}

let default =
  {
    id = -1;
    line = -1;
    file_name = "";
    file_path = "";
    cil = None;
    pdata = Parse.empty_run;
    code = "";
    inspect = None;
    selected_view = SelectedView.Content;
    selected_sidebar = Selected_sidebar.State;
    selected_panel = None;
    syntactic_search = Syntactic_search.default;
  }

let cil state = state.cil

let pdata state = state.pdata

let inspect state = Option.get state.inspect

let inspect_opt state = state.inspect

let selected_view state = state.selected_view

let selected_sidebar state = state.selected_sidebar

let selected_panel state = Option.get state.selected_panel

let selected_panel_opt state = state.selected_panel

let get_syntactic_search state = state.syntactic_search

type action =
  | Set_id of int
  | Set_line of int
  | Set_file_name of string
  | Set_file_path of string
  | Set_cil of Cil.file
  | Set_pdata of Parse.run
  | Set_code of string
  | Set_selected_view of SelectedView.t
  | Inspect_file of string * string
  | Update_code of string
  | Inspect_function of string * string * string
  | Update_dot of string
  | Reset_inspect
  | Switch_sidebar of selected_sidebar
  | Switch_panel of selected_panel option
  | Update_query of string
  | Execute_query
  | Clear_matches

let reducer (state : t) (act : action) =
  match act with
  | Set_id id -> { state with id }
  | Set_line line -> { state with line }
  | Set_file_name file_name -> { state with file_name }
  | Set_file_path file_path -> { state with file_path }
  | Set_cil cil -> { state with cil = Some cil }
  | Set_pdata pdata -> { state with pdata }
  | Set_code code -> { state with code }
  | Set_selected_view selected_view -> { state with selected_view }
  | Inspect_file (name, path) ->
      let inspect = Some (Inspect.File { name; path; code = None }) in
      { state with file_name = name; file_path = path; inspect }
  | Update_code code -> (
      match state.inspect with
      | Some (File f) ->
          let inspect = Some (Inspect.File { f with code = Some code }) in
          { state with code; inspect }
      | _ -> state )
  | Inspect_function (name, file_name, file_path) ->
      let inspect =
        Some (Inspect.Func { name; file_name; file_path; dot = None })
      in
      { state with inspect }
  | Update_dot dot -> (
      match state.inspect with
      | Some (Func f) ->
          let inspect = Some (Inspect.Func { f with dot = Some dot }) in
          { state with inspect }
      | _ -> state )
  | Reset_inspect -> { state with inspect = None }
  | Switch_sidebar selected_sidebar -> { state with selected_sidebar }
  | Switch_panel selected_panel -> { state with selected_panel }
  | Update_query query_text ->
      let q = Syntactic_search.try_to_parse query_text in
      let ss = { state.syntactic_search with query_text; query = Some q } in
      { state with syntactic_search = ss }
  | Execute_query -> (
      let ss = state.syntactic_search in
      match (state.cil, ss.query) with
      | Some c, Some (Ok q) ->
          let m = QueryMapping.map_query q c in
          let ss' = { ss with matches = Some m } in
          { state with syntactic_search = ss' }
      | _ -> state )
  | Clear_matches ->
      let ss = { state.syntactic_search with matches = None } in
      { state with syntactic_search = ss }
