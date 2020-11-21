type t =
  { id: int
  ; line: int
  ; file_name: string
  ; file_path: string
  ; pdata: Parse.run
  ; code: string
  ; selected_view: SelectedView.t }

let default =
  { id= -1
  ; line= -1
  ; file_name= ""
  ; file_path= ""
  ; pdata= Parse.empty_run
  ; code= ""
  ; selected_view= SelectedView.Code }

type action =
  | Set_id of int
  | Set_line of int
  | Set_file_name of string
  | Set_file_path of string
  | Set_pdata of Parse.run
  | Set_code of string
  | Set_selected_view of SelectedView.t

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
