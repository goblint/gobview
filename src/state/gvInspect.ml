module Line = struct
  type t = string * int

  let of_location (loc : GoblintCil.location) = (loc.file, loc.line)
end

module Node = struct
  type t = string
end

type line = Line.t

type node = Node.t

type t = Line of line | Node of node

let line file num = Line (file, num)

let node id = Node id
