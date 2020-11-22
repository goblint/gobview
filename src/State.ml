module Inspect = struct
  type file = { name : string; path : string; code : string option }

  type func = {
    name : string;
    file_name : string;
    file_path : string;
    dot : string option;
  }
end

type inspect = File of Inspect.file | Func of Inspect.func

type t = {
  id : int;
  line : int;
  file_name : string;
  file_path : string;
  pdata : Parse.run;
  code : string;
  inspect : inspect option;
  selected_view : SelectedView.t;
}

let default =
  {
    id = -1;
    line = -1;
    file_name = "";
    file_path = "";
    pdata = Parse.empty_run;
    code = "";
    inspect = None;
    selected_view = SelectedView.Code;
  }

type action =
  | Set_id of int
  | Set_line of int
  | Set_file_name of string
  | Set_file_path of string
  | Set_pdata of Parse.run
  | Set_code of string
  | Set_selected_view of SelectedView.t
  | Inspect_file of string * string
  | Update_code of string
  | Inspect_function of string * string * string
  | Update_dot of string
  | Reset_inspect

let reducer (state : t) (act : action) =
  match act with
  | Set_id id -> { state with id }
  | Set_line line -> { state with line }
  | Set_file_name file_name -> { state with file_name }
  | Set_file_path file_path -> { state with file_path }
  | Set_pdata pdata -> { state with pdata }
  | Set_code code -> { state with code }
  | Set_selected_view selected_view -> { state with selected_view }
  | Inspect_file (name, path) ->
      let inspect = Some (File { name; path; code = None }) in
      { state with file_name = name; file_path = path; inspect }
  | Update_code code -> (
      match state.inspect with
      | Some (File f) ->
          let inspect = Some (File { f with code = Some code }) in
          { state with code; inspect }
      | _ -> state )
  | Inspect_function (name, file_name, file_path) ->
      let inspect = Some (Func { name; file_name; file_path; dot = None }) in
      { state with inspect }
  | Update_dot dot -> (
      match state.inspect with
      | Some (Func f) ->
          let inspect = Some (Func { f with dot = Some dot }) in
          { state with inspect }
      | _ -> state )
  | Reset_inspect -> { state with inspect = None }
