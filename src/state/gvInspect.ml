module Line = struct
  type t = string * int

  let empty = ("", -1)

  let make ~file ~num = (file, num)

  let valid (_, n) = n != -1

  let of_location (loc : Cil.location) = (loc.file, loc.line)
end

module Node = struct
  type t = string
end

type line = Line.t

type node = Node.t

type t = Line of line | Node of node

let line ~file ~num = Line (file, num)

let line_num = function Line ((_, n) as line) when Line.valid line -> Some n | _ -> None

let node ~id = Node id
