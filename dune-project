(lang dune 2.7)

(name gobview)

(license MIT)

(maintainers "Javier Chávarri <javier.chavarri@gmail.com>")

(authors "Javier Chávarri <javier.chavarri@gmail.com>")

(source
 (github jchavarri/jsoo-react))

(generate_opam_files true)

(implicit_transitive_deps false)

(package
 (name gobview)
 (synopsis "Bindings to ReactJS for js_of_ocaml, including JSX ppx")
 (depends
  (dune (and (>= 2) (< 3)))
  (ocaml (and (>= 4.10.0) (< 4.11.0)))

  xml-light

  zarith
  zarith_stubs_js

  (ocamlformat :dev)
))
