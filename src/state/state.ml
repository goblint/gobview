open Batteries
module Inspect = InspectState

type inspect = Inspect.t

module SelectedSidebar = struct
  type t = State | OldGlobals | Nodes | Globals
end

type selected_sidebar = SelectedSidebar.t

module Selected_panel = struct
  type t = Warnings | Dead_code | Parameters | Statistics
end

type selected_panel = Selected_panel.t

type warning = [ `text of string * Cil.location | `group of string * (string * Cil.location) list ]

type t = {
  id : int;
  line : int;
  file_name : string;
  file_path : string;
  cil : Cil.file option;
  goblint : GvGoblint.solver_state;
  meta : Yojson.Safe.t;
  warnings : warning list;
  pdata : Parse.run;
  code : string;
  inspect : inspect option;
  selected_sidebar : selected_sidebar;
  selected_panel : selected_panel option;
  search : Search.t;
}

let default =
  {
    id = -1;
    line = -1;
    file_name = "";
    file_path = "";
    pdata = Parse.empty_run;
    code = "";
    goblint = GvGoblint.empty;
    cil = None;
    meta = `Null;
    warnings = [];
    inspect = None;
    selected_sidebar = SelectedSidebar.State;
    selected_panel = None;
    search = Search.default;
  }

let create ~pdata ~cil ~goblint ~meta ~warnings () =
  { default with pdata; cil = Some cil; goblint; meta; warnings }

let cil state = state.cil

let pdata state = state.pdata

let selected_sidebar state = state.selected_sidebar

let selected_panel state = Option.get state.selected_panel

let selected_panel_opt state = state.selected_panel
