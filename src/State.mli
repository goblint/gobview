module Inspect : sig
  module File : sig
    type t

    val name : t -> string

    val path : t -> string

    val code : t -> string

    val code_opt : t -> string option
  end

  module Func : sig
    type t

    val name : t -> string

    val file_name : t -> string

    val file_path : t -> string

    val dot : t -> string

    val dot_opt : t -> string option
  end

  type file = File.t

  type func = Func.t

  type t = File of file | Func of func
end

type inspect = Inspect.t

module Selected_sidebar : sig
  type t = State | Globals
end

type selected_sidebar = Selected_sidebar.t

module Selected_panel : sig
  type t = Warnings | Dead_code | Parameters | Statistics
end

type selected_panel = Selected_panel.t

type t = {
  id : int;
  line : int;
  file_name : string;
  file_path : string;
  pdata : Parse.run;
  code : string;
  inspect : inspect option;
  selected_view : SelectedView.t;
  selected_sidebar : selected_sidebar;
  selected_panel : selected_panel option;
}

val default : t

val pdata : t -> Parse.run

val inspect : t -> inspect

val inspect_opt : t -> inspect option

val selected_view : t -> SelectedView.t

val selected_sidebar : t -> selected_sidebar

val selected_panel : t -> selected_panel

val selected_panel_opt : t -> selected_panel option

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
  | Switch_sidebar of selected_sidebar
  | Switch_panel of selected_panel option

val reducer : t -> action -> t
