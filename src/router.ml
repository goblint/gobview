open Cohttp

type 'a route = Code.meth * (string -> 'a option)
type 'a t = 'a route list ref

let add (router : 'a t) meth fmt f =
  let matcher path = try Some (Scanf.sscanf path fmt f) with Scanf.Scan_failure _ -> None in
  router := (meth, matcher) :: !router;
  router

let route (router : 'a t) req =
  let meth = Request.meth req in
  let uri = Request.uri req in
  let path = Uri.path uri in
  List.find_map (fun (m, f) -> if m = meth then f path else None) !router

let make () : 'a t = ref []
