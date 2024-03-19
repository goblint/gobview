open Batteries
open GoblintCil

module SelectedSidebar = struct
  type t = Nodes | Globals | Files | Search
end

type selected_sidebar = SelectedSidebar.t

module SelectedPanel = struct
  type t = Warnings | DeadCode | Parameters | Statistics
end

type selected_panel = SelectedPanel.t

type display = GvDisplay.t

type inspect = GvInspect.t

type t = {
  cil : Cil.file;
  goblint : GvGoblint.solver_state;
  warnings : GvMessages.t;
  meta : Yojson.Safe.t;
  stats : Goblint_timing.tree * Gc.stat;
  file_loc : (string, string) Hashtbl.t;
  display : display option;
  inspect : inspect option;
  selected_sidebar_left : selected_sidebar;
  selected_sidebar_right : selected_sidebar;
  selected_panel : selected_panel option;
  search : Search.t;
}

let timing_default : Goblint_timing.tree = {
  name = "";
  cputime = 0.0;
  walltime = 0.0;
  allocated = 0.0;
  count = 0;
  children = [];
}
let default =
  {
    cil = Cil.dummyFile;
    goblint = GvGoblint.empty;
    warnings = [];
    meta = `Null;
    stats = (timing_default, Gc.quick_stat ());
    file_loc = Hashtbl.create 113;
    display = None;
    inspect = None;
    selected_sidebar_left = SelectedSidebar.Files;
    selected_sidebar_right = SelectedSidebar.Nodes;
    selected_panel = None;
    search = Search.default;
  }

let make ~file ~cil ~goblint ~warnings ~meta ~stats ~file_loc () =
  { default with display=Some(File(file)); cil; goblint; warnings; meta; stats; file_loc }
