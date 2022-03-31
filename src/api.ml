open Batteries
open State

module type Request = sig
  val name: string

  type body
  type response

  val body_of_yojson: Yojson.Safe.t -> body
  val yojson_of_response: response -> Yojson.Safe.t

  val process: State.t -> body -> response Lwt.t
end

let registry = Hashtbl.create 16

let register (module R : Request) = Hashtbl.add registry R.name (module R : Request)

module Ping = struct
  let name = "ping"
  type body = unit [@@deriving yojson]
  type response = unit [@@deriving yojson]
  let process state () = Goblint.ping state.goblint
end

module Config = struct
  let name = "config"
  type body = string * Json.t [@@deriving yojson]
  type response = unit [@@deriving yojson]
  let process state (conf, json) = Goblint.config state.goblint conf json
end

let () =
  register (module Ping);
  register (module Config)
