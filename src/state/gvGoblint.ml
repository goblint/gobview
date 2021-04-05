open Batteries
open Cil

type local = [ `Line of string * int | `Node of string ]

class virtual solver_state =
  object
    method virtual local_analyses : local -> (string * (Representation.t * Representation.t)) list

    method virtual globs : Parse.glob list

    method virtual global_names : string list

    method virtual global : string -> Representation.t

    method virtual global_analyses : (string * (string * Representation.t) list) list
  end

class empty_solver_state =
  object
    inherit solver_state

    method local_analyses _ = []

    method globs = []

    method global_names = []

    method global _ = `Nothing

    method global_analyses = []
  end

module type Sig = sig
  type t

  val wrap_solver_state : t -> solver_state
end

module Make (Cfg : MyCFG.CfgBidir) (Spec : Analyses.SpecHC) : Sig = struct
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
           [ (`Line (LVar.file_name k, LVar.line_nr k), (id, c, v)); (`Node id, (id, c, v)) ])
    |> Enum.map List.enum |> Enum.concat |> Hashtbl.of_enum

  let local_analyses lh' l =
    Hashtbl.find_all lh' l
    |> List.map (fun (id, c, d) -> (id, (Spec.C.represent c, LSpec.represent d)))

  let parse s =
    let parser = XmlParser.make () in
    XmlParser.prove parser false;
    XmlParser.parse parser (SString s)

  let glob (v, s) =
    let f = IO.output_string () in
    GSpec.printXml f s;
    let xml = IO.close_out f in
    "<glob><key>" ^ v.vname ^ "</key>" ^ xml ^ "</glob>" |> parse |> Parse.parse_glob

  let globs ((_, gh) : t) = GHashtbl.enum gh |> Enum.map glob |> List.of_enum

  let transform_ghashtbl = Hashtbl.of_enum % Enum.map (fun (k, v) -> (k.vname, v)) % GHashtbl.enum

  let global_names = List.of_enum % Enum.map (fun (k, _) -> k.vname) % GHashtbl.enum

  let global n gh' =
    Hashtbl.find_option gh' n |> Option.map_default (fun g -> GSpec.represent g) `Nothing

  let compute_global_analysis_tbl gh =
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

      val global_analysis_tbl = compute_global_analysis_tbl gh

      method local_analyses = local_analyses lh'

      method globs = globs (lh, gh)

      method global_names = global_names gh

      method global n = global n gh'

      method global_analyses = global_analysis_tbl
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
