open Batteries

module SelectedSidebar = struct
  type t = Nodes | Globals
end

type selected_sidebar = SelectedSidebar.t

module Selected_panel = struct
  type t = Warnings | Dead_code | Parameters | Statistics
end

type selected_panel = Selected_panel.t

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
    cil = Cil.dummyFile;
    meta = `Null;
    warnings = [];
    display = None;
    inspect = None;
    selected_sidebar = SelectedSidebar.Nodes;
    selected_panel = None;
    search = Search.default;
  }

let make ~pdata ~cil ~goblint ~meta ~warnings () =
  { default with pdata; cil; goblint; meta; warnings }
