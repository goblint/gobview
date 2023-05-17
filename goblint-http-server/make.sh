#!/usr/bin/env bash
set -e # exit immediately if a command fails
set -o pipefail # or all $? in pipe instead of returning exit code of the last command only

#$TARGET=src/goblint_http

rule() {
  case $1 in
    # new rules using dune
    clean)
      git clean -X -f
      dune clean
    ;; setup)
      # set -x
      # opam init -y -a --bare $SANDBOXING # sandboxing is disabled in travis and docker
      # opam update
      opam switch -y create . --deps-only ocaml-base-compiler.4.13.1 ocaml-options-vanilla --locked
    ;; nat*)
      eval $(opam config env)
      dune build #$TARGET.exe &&
      rm -f goblint-http &&
      cp _build/install/default/bin/goblint-http goblint-http
    ;; dev)
      eval $(opam env)
      echo "Installing opam packages for development..."
      opam install -y utop ocaml-lsp-server ocp-indent ocamlformat ounit2
      # ocaml-lsp-server is needed for https://github.com/ocamllabs/vscode-ocaml-platform
      echo "Be sure to adjust your vim/emacs config!"
  esac;
}

if [ $# -eq 0 ]; then
  rule native
else
  while [ $# -gt 0 ]; do
    rule $1;
    shift
  done
fi
