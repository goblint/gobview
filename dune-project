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
  (dune
   (< 3))
  (ocaml
   (>= 4.10.0))
  jsoo-react
  goblint-cil
  zarith_stubs_js
  (ocamlformat :dev)))
