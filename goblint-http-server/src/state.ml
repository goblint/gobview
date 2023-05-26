type t = {
  goblint: Goblint.t;
  (* Descriptor for the Goblint server instance *)
  mutable save_run: string option;
  (* The directory from which we serve the marshalled analysis state *)
}

let make (goblint: Goblint.t) = { goblint; save_run = None }
