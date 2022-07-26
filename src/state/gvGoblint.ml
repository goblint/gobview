open Batteries
open GoblintCil

let rec representation_of_yojson (json : Yojson.Safe.t) : Representation.t =
  match json with
  | `Tuple l -> l |> List.map representation_of_yojson |> Representation.list
  | `Bool b -> Bool.to_string b |> Representation.value
  | `Intlit lit -> lit |> Representation.value
  | `Null -> Representation.nothing
  | `Variant ("Bot", v) ->
      Representation.tagged "bot"
        (Option.map_default representation_of_yojson (Representation.value "⊥") v)
  | `Variant ("Top", v) ->
      Representation.tagged "top"
        (Option.map_default representation_of_yojson (Representation.value "⊤") v)
  | `Variant ("Set", Some (`List l)) ->
      Representation.tagged "set" (representation_of_yojson (`List l))
  | `Variant (n, None) -> Representation.value n
  | `Variant (n, Some v) -> Representation.tagged n (representation_of_yojson v)
  | `Assoc l ->
      l |> List.map (fun (k, v) -> (k, representation_of_yojson v)) |> Representation.assoc
  | `List l -> l |> List.map representation_of_yojson |> Representation.list
  | `Float f -> Float.to_string f |> Representation.value
  | `String s -> Representation.value s
  | `Int i -> Int.to_string i |> Representation.value

class virtual solver_state =
  object
    method virtual local_analyses
        : GvInspect.t -> (string * (Representation.t * Representation.t)) list

    method virtual has_local_analysis : GvInspect.line -> bool

    method virtual global_analyses : (string * (string * Representation.t) list) list

    method virtual dead_locations : location list

    method virtual is_dead : file:string -> line:int -> bool

    method virtual dot_of_fundec : Cil.fundec -> string
  end

class empty_solver_state =
  object
    inherit solver_state

    method local_analyses _ = []

    method has_local_analysis _ = false

    method global_analyses = []

    method dead_locations = []

    method is_dead ~file:_ ~line:_ = false

    method dot_of_fundec _ = ""
  end

module type Sig = sig
  type t

  val wrap_solver_state : unit -> solver_state
end

module Make
    (Cfg : MyCFG.CfgBidir)
    (Spec : Analyses.Spec) (Inc : sig
      val increment : Analyses.increment_data
    end) : Sig = struct
  module A = Control.AnalyzeCFG (Cfg) (Spec) (Inc)
  module EQSys = A.EQSys
  module LVar = EQSys.LVar
  module LSpec = Spec.D
  module LHT = A.LHT
  module GVar = EQSys.GVar
  module GSpec = Spec.G
  module GHT = A.GHT

  type t = Spec.D.t LHT.t * Spec.G.t GHT.t

  let transform_lht lh =
    lh |> LHT.enum
    |> Enum.map (fun (((_, c) as k), v) ->
           let id = LVar.var_id k in
           let loc = LVar.getLocation k in
           [ (GvInspect.Line (loc.file, loc.line), (id, c, v)); (GvInspect.Node id, (id, c, v)) ])
    |> Enum.map List.enum |> Enum.concat |> Hashtbl.of_enum

  let local_analyses lh' l =
    Hashtbl.find_all lh' l
    |> List.map (fun (id, c, d) ->
           ( id,
             ( Spec.C.to_yojson c |> representation_of_yojson,
               LSpec.to_yojson d |> representation_of_yojson ) ))

  let has_local_analysis lh' l = Hashtbl.mem lh' (GvInspect.Line l)

  let dead_locations lh =
    let module NodeSet = Set.Make (Node) in
    let dead = ref NodeSet.empty in
    let live = ref NodeSet.empty in
    lh
    |> LHT.iter (fun (n, _) d ->
           if Spec.D.is_bot d then dead := NodeSet.add n !dead else live := NodeSet.add n !live);
    NodeSet.diff !dead !live |> NodeSet.to_list |> List.map Node.location

  let transform_ght gh =
    let tbl = Hashtbl.create (GHT.length gh) in
    let insert_analysis_result a v r =
      Hashtbl.modify_opt a (function None -> Some [ (v, r) ] | Some l -> Some ((v, r) :: l)) tbl
    in
    let f k v =
      match v with
      | `Assoc l -> List.iter (fun (a, r) -> insert_analysis_result a (EQSys.GVar.show k) r) l
      | _ -> failwith "Not sure if this is supposed to happen."
    in
    gh |> GHT.map (fun _ -> representation_of_yojson % EQSys.G.to_yojson) |> GHT.iter f;
    Hashtbl.to_list tbl

  let dot_of_fundec (fd : Cil.fundec) =
    CfgTools.sprint_fundec_html_dot (module Cfg : MyCFG.CfgBidir) (fun _ -> true) fd;

  class solver_state_impl (lh, gh) =
    object
      inherit solver_state

      val lh' = transform_lht lh

      val gh' = transform_ght gh

      val dead = dead_locations lh

      method local_analyses = local_analyses lh'

      method has_local_analysis = has_local_analysis lh'

      method global_analyses = gh'

      method dead_locations = dead

      method is_dead ~file ~line = List.exists (fun loc -> loc.file = file && line = loc.line) dead

      method dot_of_fundec = dot_of_fundec
    end

  let wrap_solver_state () =
    let module Arg = struct
      include A.PostSolverArg

      let should_prune = false
    end in
    let module S2' =
      Constraints.GlobSolverFromEqSolver (Generic.LoadRunIncrSolver (Arg)) (EQSys) (LHT) (GHT)
    in
    let r2, _ = S2'.solve [] [] [] in
    new solver_state_impl r2
end

let empty : solver_state = new empty_solver_state

let unmarshal spec cil =
  let (module Cfg) = Control.compute_cfg cil in
  let (module Spec) = Control.get_spec () in
  Spec.init (Some (Marshal.from_string spec 0));
  let (module G : Sig) =
    (module Make (Cfg) (Spec)
              (struct
                let increment = Analyses.empty_increment_data ()
              end))
  in
  G.wrap_solver_state ()
