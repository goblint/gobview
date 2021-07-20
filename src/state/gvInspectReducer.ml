open Batteries
module I = GvInspect

let reduce (_ : State.t) (_ : I.t option) = function
  | `InspectLine (file, num) -> I.line file num |> Option.some
  | `InspectNode id -> I.node id |> Option.some
