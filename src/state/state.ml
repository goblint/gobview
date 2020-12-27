module Inspect = struct
  module File = struct
    type t = { name : string; path : string; code : string option }

    type location =
      | By_parse_file of Parse.file
      | By_cil_location of Cil.location

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
