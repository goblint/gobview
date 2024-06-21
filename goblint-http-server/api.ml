open Batteries
open State

module type Request = sig
  val name: string

  type body [@@deriving of_yojson]
  type response [@@deriving to_yojson]

  val process: State.t -> body -> response Lwt.t
end

let registry = Hashtbl.create 16

let register (module R : Request) = Hashtbl.add registry R.name (module R : Request)

module Ping = struct
  let name = "ping"
  type body = unit [@@deriving of_yojson]
  type response = unit [@@deriving to_yojson]
  let process state () = Goblint.ping state.goblint
end

module Config = struct
  let name = "config"
  type body = string * Json.t [@@deriving of_yojson]
  type response = unit [@@deriving to_yojson]
  let process state (conf, json) = Goblint.config state.goblint conf json
end

module Analyze = struct
  let name = "analyze"
  type body = [`All | `Functions of string list] option [@@deriving of_yojson]
  type response = unit [@@deriving to_yojson]
  let process state reanalyze =
    let%lwt save_run = Goblint.analyze state.goblint ?reanalyze in
    state.save_run <- Some save_run;
    Lwt.return_unit
end

let () =
  register (module Ping);
  register (module Config);
  register (module Analyze)
