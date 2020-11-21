type t =
  { id: int
  ; line: int
  ; file_name: string
  ; file_path: string
  ; pdata: Parse.run
  ; code: string
  ; function_name: string option
  ; dot: string option
  ; selected_view: SelectedView.t }

let default =
  { id= -1
  ; line= -1
  ; file_name= ""
  ; file_path= ""
  ; pdata= Parse.empty_run
  ; code= ""
  ; function_name= None
  ; dot= None
  ; selected_view= SelectedView.Code }

let function_name state = Option.get state.function_name

let function_name_opt state = state.function_name

let dot state = Option.get state.dot

let dot_opt state = state.dot

type action =
  | Set_id of int
  | Set_line of int
  | Set_file_name of string
  | Set_file_path of string
  | Set_pdata of Parse.run
  | Set_code of string
  | Set_selected_view of SelectedView.t
  | Inspect_function of string * string
  | Update_dot of string

let reducer (state : t) (act : action) =
  match act with
  | Set_id id ->
      {state with id}
  | Set_line line ->
      {state with line}
  | Set_file_name file_name ->
      {state with file_name}
  | Set_file_path file_path ->
      {state with file_path}
  | Set_pdata pdata ->
      {state with pdata}
  | Set_code code ->
      {state with code}
  | Set_selected_view selected_view ->
      {state with selected_view}
  | Inspect_function (file_path, f) ->
      let function_name = Some f in
      {state with file_path; function_name}
  | Update_dot d ->
      {state with dot= Some d}
