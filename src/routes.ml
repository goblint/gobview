open Cohttp_lwt
open Cohttp_lwt_unix

type route = (Response.t * Body.t) Lwt.t

let ping _ = Server.respond_string ~status:`OK ~body:"Pong" ()

let router data =
  let router = Router.make () in
  Router.add router `GET "/ping" (ping data)
