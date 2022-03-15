open Cohttp

type route = Code.meth * (string -> string option)
type t = route list ref

let add (router : t) meth fmt f =
  let matcher path = try Some (Scanf.sscanf path fmt f) with Scanf.Scan_failure _ -> None in
  router := (meth, matcher) :: !router;
  router

let make () : t = ref []
