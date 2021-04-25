open Batteries

module SelectedSidebar = struct
  type t = Nodes | Globals
end

type selected_sidebar = SelectedSidebar.t

module SelectedPanel = struct
  type t = Warnings | DeadCode | Parameters | Statistics
end

type selected_panel = SelectedPanel.t

module Warning = struct
  type t = [ `text of string * Cil.location | `group of string * (string * Cil.location) list ]

  let to_string = function
    | `text (s, _) -> s
    | `group (s, l) -> List.map (Printf.sprintf "%s: %s" s % fst) l |> String.concat "\n"

  let to_list = function
    | `text w -> [ w ]
    | `group (n, l) -> List.map (fun (s, loc) -> (Printf.sprintf "%s: %s" n s, loc)) l

  let find_all (file, line) =
    let is_this_location (loc : Cil.location) = String.equal file loc.file && line = loc.line in
    let f = function
      | `text (_, loc) as w -> if is_this_location loc then Some w else None
      | `group (s, l) ->
          let l' = List.filter (is_this_location % snd) l in
          if List.is_empty l' then None else Some (`group (s, l'))
    in
    List.filter_map f
end

type warning = Warning.t

type display = GvDisplay.t

type inspect = GvInspect.t

type t = {
  cil : Cil.file;
  goblint : GvGoblint.solver_state;
  warnings : warning list;
  meta : Yojson.Safe.t;
  stats : Stats.t * Gc.stat;
  display : display option;
  inspect : inspect option;
  selected_sidebar : selected_sidebar;
  selected_panel : selected_panel option;
  search : Search.t;
}

let default =
  {
    cil = Cil.dummyFile;
    goblint = GvGoblint.empty;
    warnings = [];
    meta = `Null;
    stats = (Stats.top, Gc.quick_stat ());
    display = None;
    inspect = None;
    selected_sidebar = SelectedSidebar.Nodes;
    selected_panel = None;
    search = Search.default;
  }

let make ~cil ~goblint ~warnings ~meta ~stats () =
  { default with cil; goblint; warnings; meta; stats }
