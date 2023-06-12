(lang dune 2.7)

(name gobview)

(license MIT)

(maintainers
 "Kerem Cakirer <kerem.cakirer@tum.de>"
 "Michael Schwarz <m.schwarz@tum.de>")

(authors
 "Kerem Cakirer <kerem.cakirer@tum.de>"
 "Alex Micheli <c1707-fisch@yahoo.de>")

(source
 (github goblint/gobview))

(generate_opam_files true)

(implicit_transitive_deps false)

(package
 (name gobview)
 (synopsis "Web frontend for Goblint")
 (depends
  dune
  (ocaml (>= 4.10.0))
  batteries
  cohttp-lwt
  cohttp-lwt-unix
  cohttp-server-lwt-unix
  fileutils
  jsonrpc
  lwt
  lwt_ppx
  yojson
  ppx_yojson_conv ; TODO: switch to ppx_deriving_yojson like Goblint itself
  conduit-lwt-unix
  jsoo-react
  (goblint-cil (>= 2.0.0))
  ctypes_stubs_js
  integers_stubs_js
  zarith_stubs_js
  (ocamlformat :dev)))
