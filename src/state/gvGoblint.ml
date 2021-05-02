open Batteries
open Cil

class virtual solver_state =
  object
    method virtual local_analyses
        : GvInspect.t -> (string * (Representation.t * Representation.t)) list

    method virtual has_local_analysis : GvInspect.line -> bool

    method virtual global_analyses : (string * (string * Representation.t) list) list

    method virtual dead_locations : location list

    method virtual is_dead : file:string -> line:int -> bool
  end

class empty_solver_state =
  object
    inherit solver_state

    method local_analyses _ = []

    method has_local_analysis _ = false

    method global_analyses = []

    method dead_locations = []

    method is_dead ~file:_ ~line:_ = false
  end

module type Sig = sig
  type t

  val wrap_solver_state : t -> solver_state
end

module Make (Cfg : MyCFG.CfgBidir) (Spec : Analyses.Spec) : Sig = struct
  module Inc = struct
    let increment = Analyses.empty_increment_data ()
  end

  module EqSys = Constraints.FromSpec (Spec) (Cfg) (Inc)
  module LVar = EqSys.LVar
  module LSpec = Spec.D
  module LHashtbl = Hashtbl.Make (LVar)
  module GVar = EqSys.GVar
  module GSpec = Spec.G
  module GHashtbl = Hashtbl.Make (GVar)

  type t = Spec.D.t LHashtbl.t * Spec.G.t GHashtbl.t

  let transform_lhashtbl lh =
    lh |> LHashtbl.enum
    |> Enum.map (fun (((_, c) as k), v) ->
           let id = LVar.var_id k in
           [
             (GvInspect.Line (LVar.file_name k, LVar.line_nr k), (id, c, v));
             (GvInspect.Node id, (id, c, v));
           ])
    |> Enum.map List.enum |> Enum.concat |> Hashtbl.of_enum

  let local_analyses lh' l =
    Hashtbl.find_all lh' l
    |> List.map (fun (id, c, d) -> (id, (Spec.C.represent c, LSpec.represent d)))

  let has_local_analysis lh' l = Hashtbl.mem lh' (GvInspect.Line l)

  let dead_locations lh =
    let module NodeSet = Set.Make (Node.Node) in
    let dead = ref NodeSet.empty in
    let live = ref NodeSet.empty in
    lh
    |> LHashtbl.iter (fun (n, _) d ->
           if Spec.D.is_bot d then dead := NodeSet.add n !dead else live := NodeSet.add n !live);
    NodeSet.diff !dead !live |> NodeSet.to_list
    |> List.map (function
         | Node.Statement stmt -> get_stmtLoc stmt.skind
         | FunctionEntry vi | Function vi -> vi.vdecl)

  let transform_ghashtbl gh =
    let tbl = Hashtbl.create (GHashtbl.length gh) in
    let insert_analysis_result a v r =
      Hashtbl.modify_opt a (function None -> Some [ (v, r) ] | Some l -> Some ((v, r) :: l)) tbl
    in
    let f k v =
      match v with
      | `Assoc l -> List.iter (fun (a, r) -> insert_analysis_result a k.vname r) l
      | _ -> failwith "Not sure if this is supposed to happen."
    in
    gh |> GHashtbl.map (fun _ -> GSpec.represent) |> GHashtbl.iter f;
    Hashtbl.to_list tbl

  class solver_state_impl (lh, gh) =
    object
      inherit solver_state

      val lh' = transform_lhashtbl lh

      val gh' = transform_ghashtbl gh

      val dead = dead_locations lh

      method local_analyses = local_analyses lh'

      method has_local_analysis = has_local_analysis lh'

      method global_analyses = gh'

      method dead_locations = dead

      method is_dead ~file ~line = List.exists (fun loc -> loc.file = file && line = loc.line) dead
    end

  let wrap_solver_state ((lh, gh) : t) = new solver_state_impl (lh, gh)
end

let empty : solver_state = new empty_solver_state

let unmarshal ~goblint cil =
  let (module Cfg) = Control.compute_cfg cil in
  let (module Spec) = Control.get_spec () in
  Spec.init ();
  Spec.finalize ();
  let (module G : Sig) = (module Make (Cfg) (Spec)) in
  let state = Marshal.from_string goblint 0 in
  G.wrap_solver_state state
