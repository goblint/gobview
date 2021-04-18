module SelectedSidebar = struct
  type t = State | OldGlobals | Nodes | Globals
end

type selected_sidebar = SelectedSidebar.t

module Selected_panel = struct
  type t = Warnings | Dead_code | Parameters | Statistics
end

type selected_panel = Selected_panel.t

type warning = [ `text of string * Cil.location | `group of string * (string * Cil.location) list ]

type display = GvDisplay.t

type t = {
  id : int;
  line : int;
  file_name : string;
  file_path : string;
  code : string;
  cil : Cil.file;
  goblint : GvGoblint.solver_state;
  meta : Yojson.Safe.t;
  warnings : warning list;
  pdata : Parse.run;
  display : display option;
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
    cil = Cil.dummyFile;
    meta = `Null;
    warnings = [];
    display = None;
    selected_sidebar = SelectedSidebar.State;
    selected_panel = None;
    search = Search.default;
  }

let make ~pdata ~cil ~goblint ~meta ~warnings () =
  { default with pdata; cil; goblint; meta; warnings }
