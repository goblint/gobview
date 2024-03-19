open Batteries
module I = GvInspect

let reduce (_ : State.t) (_ : I.t option) = function
  | `InspectLine (Some (file, num)) -> I.line file num |> Option.some
  | `InspectLine None -> None
  | `InspectNode (Some id) -> I.node id |> Option.some
  | `InspectNode None -> None
