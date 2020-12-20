(lang dune 2.7)

(name gobview)

(license MIT)

(maintainers "Kerem Cakirer <kerem.cakirer@tum.de>"
  "Michael Schwarz <m.schwarz@tum.de>")

(authors "Alex Micheli <c1707-fisch@yahoo.de>")

(source
 (github keremc/goblint-webfrontend-jsoo))

(generate_opam_files true)

(implicit_transitive_deps false)

(package
 (name gobview)
 (synopsis "Web frontend for Goblint")
 (depends
  (dune
   (and
    (>= 2)
    (< 3)))
  (ocaml
   (and
    (>= 4.10.0)
    (< 4.11.0)))
  jsoo-react
  xml-light
  zarith
  zarith_stubs_js
  goblint-cil
  (ocamlformat :dev)))
