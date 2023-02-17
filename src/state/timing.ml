
(** Timing tree node. *)
type tree = {
  name: string; (** Name of node. *)
  mutable cputime: float; (** Accumulated CPU time in seconds. *)
  mutable walltime: float; (** Accumulated wall time in seconds. *)
  mutable allocated: float; (** Accumulated allocated memory in bytes. *)
  mutable count: int; (** Number of repetitions. Only set if {!Timing.countCalls} is true. *)
  mutable children: tree list; (** Child nodes. *)
}

let create_tree name =
  {
    name = name;
    cputime = 0.0;
    walltime = 0.0;
    allocated = 0.0;
    count = 0;
    children = [];
  }
