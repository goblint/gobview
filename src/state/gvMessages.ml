open Batteries
open Messages

module Message = struct
  include Message

  let location { multipiece; _ } =
    let loc (p : Piece.t) = Option.map Location.to_cil p.loc in
    match multipiece with
    | Single p -> loc p
    | Group { pieces; _ } -> pieces |> List.filter_map loc |> List.enum |> Enum.get

  let to_string msg =
    let out = IO.output_string () in
    let fmt = Format.formatter_of_output out in
    print ~ppf:fmt msg;
    IO.close_out out
end

type t = Message.t list

let filter_by_line (file, line) =
  let pred msg =
    match Message.location msg with
    | Some loc when loc.file = file && loc.line = line -> true
    | _ -> false
  in
  List.filter pred
