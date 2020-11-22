module Inspect = struct
  module File = struct
    type t = {name: string; path: string; code: string option}
  end

  module Function = struct
    type t =
      {name: string; file_name: string; file_path: string; dot: string option}

    let name f = f.name

    let dot_opt f = f.dot
  end

  type t = File of File.t | Function of Function.t

  let function_opt i = match i with Function f -> Some f | _ -> None
end

type t =
  { id: int
  ; line: int
  ; file_name: string
  ; file_path: string
  ; pdata: Parse.run
  ; code: string
  ; inspect: Inspect.t option
  ; selected_view: SelectedView.t }

let default =
  { id= -1
  ; line= -1
  ; file_name= ""
  ; file_path= ""
  ; pdata= Parse.empty_run
  ; code= ""
  ; inspect= None
  ; selected_view= SelectedView.Code }

let inspect i = Option.get i.inspect

let inspect_opt i = i.inspect

type action =
  | Set_id of int
  | Set_line of int
  | Set_file_name of string
  | Set_file_path of string
  | Set_pdata of Parse.run
  | Set_code of string
  | Set_selected_view of SelectedView.t
  | Inspect_function of string * string * string
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
  | Inspect_function (name, file_name, file_path) ->
      let inspect =
        Some (Inspect.Function {name; file_name; file_path; dot= None})
      in
      {state with inspect}
  | Update_dot dot -> (
    match state.inspect with
    | Some (Function f) ->
        let inspect = Some (Inspect.Function {f with dot= Some dot}) in
        {state with inspect}
    | _ ->
        state )
