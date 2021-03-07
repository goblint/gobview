open Batteries
open Cil

class virtual solver_state =
  object
    method virtual globs : Parse.glob list
  end

class empty_solver_state =
  object
    inherit solver_state

    method globs = []
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

  class solver_state_impl (lh, gh) =
    object
      inherit solver_state

      method globs = globs (lh, gh)
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
