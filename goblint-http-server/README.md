# Http-Server for GobView

## Building
1. Install [opam](https://opam.ocaml.org/doc/Install.html).
2. Run `make setup` to install OCaml and dependencies via opam.
4. Run `make` to build the Goblint-Http-Server itself.

## Running
To use the Goblint Http-Server run e.g.:
`./goblint-http -with-goblint ../analyzer/goblint -goblint --set save_run run --set files[+] "../analyzer/tests/regression/00-sanity/01-assert.c"`
For more information on the usage of `goblint-http` run:
`./goblint-http -help`
