open Batteries

module Line = struct
  type t = string * int

  let valid (_, n) = n != -1

  let of_location (loc : Cil.location) = (loc.file, loc.line)
end

module Node = struct
  type t = string
end
