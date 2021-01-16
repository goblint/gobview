external unsafe_coerce : 'a -> 'b = "%identity"

(* jsoo-react seems to have some trouble with non-mandatory component
   properties. This is a workaround until the issue is resolved. *)
let fix_opt_arg (x : 'a option) : 'a option =
  let x = unsafe_coerce x in
  (* Coerce x from 'a option to 'a optdef, which is its actual type! *)
  Js_of_ocaml.Js.Optdef.to_option x

let fix_opt_args2 (a, b) = (fix_opt_arg a, fix_opt_arg b)

let fix_opt_args3 (a, b, c) = (fix_opt_arg a, fix_opt_arg b, fix_opt_arg c)

let fix_opt_args4 (a, b, c, d) =
  (fix_opt_arg a, fix_opt_arg b, fix_opt_arg c, fix_opt_arg d)

let fix_opt_args5 (a, b, c, d, e) =
  (fix_opt_arg a, fix_opt_arg b, fix_opt_arg c, fix_opt_arg d, fix_opt_arg e)

let fix_opt_args6 (a, b, c, d, e, f) =
  ( fix_opt_arg a,
    fix_opt_arg b,
    fix_opt_arg c,
    fix_opt_arg d,
    fix_opt_arg e,
    fix_opt_arg f )

let fix_opt_args7 (a, b, c, d, e, f, g) =
  ( fix_opt_arg a,
    fix_opt_arg b,
    fix_opt_arg c,
    fix_opt_arg d,
    fix_opt_arg e,
    fix_opt_arg f,
    fix_opt_arg g )

let fix_opt_args8 (a, b, c, d, e, f, g, h) =
  ( fix_opt_arg a,
    fix_opt_arg b,
    fix_opt_arg c,
    fix_opt_arg d,
    fix_opt_arg e,
    fix_opt_arg f,
    fix_opt_arg g,
    fix_opt_arg h )

let fix_opt_args9 (a, b, c, d, e, f, g, h, i) =
  ( fix_opt_arg a,
    fix_opt_arg b,
    fix_opt_arg c,
    fix_opt_arg d,
    fix_opt_arg e,
    fix_opt_arg f,
    fix_opt_arg g,
    fix_opt_arg h,
    fix_opt_arg i )
