module Inspect = InspectState

type inspect = Inspect.t

module Selected_sidebar = struct
  type t = State | Globals
end

type selected_sidebar = Selected_sidebar.t

module Selected_panel = struct
  type t = Warnings | Dead_code | Parameters | Statistics
end

type selected_panel = Selected_panel.t

module SyntacticSearch = SyntacticSearchState

type syntactic_search = SyntacticSearch.t

type t = {
  id : int;
  line : int;
  file_name : string;
  file_path : string;
  cil : Cil.file option;
  pdata : Parse.run;
  code : string;
  inspect : inspect option;
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
    pdata = Parse.empty_run;
    code = "";
    cil = None;
    inspect = None;
    selected_sidebar = Selected_sidebar.State;
    selected_panel = None;
    syntactic_search = SyntacticSearch.default;
  }

let create ~pdata ~cil () = { default with pdata; cil = Some cil }

let cil state = state.cil

let pdata state = state.pdata

let selected_sidebar state = state.selected_sidebar

let selected_panel state = Option.get state.selected_panel

let selected_panel_opt state = state.selected_panel
