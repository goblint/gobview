open Batteries

module type Request = sig
  val name: string

  type body
  type response

  val body_of_yojson: Yojson.Safe.t -> body
  val yojson_of_response: response -> Yojson.Safe.t

  val process: body -> response Lwt.t
end

let registry = Hashtbl.create 16

let register (module R : Request) = Hashtbl.add registry R.name (module R : Request)

module Ping = struct
  let name = "ping"
  type body = unit [@@deriving yojson]
  type response = unit [@@deriving yojson]
  let process () = Lwt.return_unit
end

let () = register (module Ping)
