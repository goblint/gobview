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
  syntactic_search : Syntactic_search_state.t;
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
    syntactic_search = Syntactic_search_state.default;
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

let reducer (s : t) (a : action) =
  match a with
  | Set_id id -> { s with id }
  | Set_line line -> { s with line }
  | Set_file_name file_name -> { s with file_name }
  | Set_file_path file_path -> { s with file_path }
  | Set_cil cil -> { s with cil = Some cil }
  | Set_pdata pdata -> { s with pdata }
  | Set_code code -> { s with code }
  | Set_selected_view selected_view -> { s with selected_view }
  | Inspect_file (name, path) ->
      let inspect = Some (Inspect.File { name; path; code = None }) in
      { s with file_name = name; file_path = path; inspect }
  | Update_code code -> (
      match s.inspect with
      | Some (File f) ->
          let inspect = Some (Inspect.File { f with code = Some code }) in
          { s with code; inspect }
      | _ -> s )
  | Inspect_function (name, file_name, file_path) ->
      let inspect =
        Some (Inspect.Func { name; file_name; file_path; dot = None })
      in
      { s with inspect }
  | Update_dot dot -> (
      match s.inspect with
      | Some (Func f) ->
          let inspect = Some (Inspect.Func { f with dot = Some dot }) in
          { s with inspect }
      | _ -> s )
  | Reset_inspect -> { s with inspect = None }
  | Switch_sidebar selected_sidebar -> { s with selected_sidebar }
  | Switch_panel selected_panel -> { s with selected_panel }
  | Update_query q ->
      let ss = s.syntactic_search in
      { s with syntactic_search = Syntactic_search_state.update_query ss q }
  | Execute_query -> (
      let ss = s.syntactic_search in
      match s.cil with
      | Some c ->
          {
            s with
            syntactic_search = Syntactic_search_state.execute_query ss c;
          }
      | _ -> s )
  | Clear_matches ->
      let ss = s.syntactic_search in
      { s with syntactic_search = Syntactic_search_state.clear_matches ss }
