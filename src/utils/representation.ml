type t =
  [ `Nothing
  | `Value of string
  | `Pair of t * t
  | `List of t list
  | `Assoc of (string * t) list
  | `Tagged of string * t ]

let rec to_yojson (r : t) : Yojson.Safe.t =
  match r with
  | `Nothing -> `Null
  | `Value s -> `String s
  | `Pair (a, b) -> `List [ to_yojson a; to_yojson b ]
  | `List [ r ] -> to_yojson r
  | `List l -> `List (BatList.map to_yojson l)
  | `Assoc l -> `Assoc (BatList.map (fun (k, v) -> (k, to_yojson v)) l)
  | `Tagged (n, r) -> `List [ `String n; to_yojson r ]

let nothing = `Nothing

let value s : t = `Value s

let pair a b : t = `Pair (a, b)

let list l : t = `List l

let assoc l : t = `Assoc l

let tagged t r : t = `Tagged (t, r)

let bot s = tagged "bot" (value s)

let top s = tagged "top" (value s)

let is_value = function `Value _ -> true | _ -> false

let rec is_simple = function
  | `Nothing -> true
  | `Value _ -> true
  | `Pair (a, b) -> is_simple a && is_simple b
  | `List [ r ] -> is_simple r
  | _ -> false
