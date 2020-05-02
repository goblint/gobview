open Tree;

let makeProps:
  (~tree: tree, ~key: string=?, unit) =>
  Js_of_ocaml.Js.t({. tree: Js_of_ocaml.Js.readonly_prop(tree)}) =
  (~tree, ~key=?, _) =>
    Js_of_ocaml.Js.Unsafe.(
      obj(
        [|
          Option.map(
            _ => (
              "key",
              inject(
                Option.map(Js_of_ocaml.Js.string, key)
                |> Js_of_ocaml.Js.Opt.option,
              ),
            ),
            key,
          ),
          Some(("tree", inject(tree))),
        |]
        |> Array.to_list
        |> List.filter_map(x => x)
        |> Array.of_list,
      )
    );
    
let rec make =
(
  props:
    Js_of_ocaml.Js.t({
      .
      tree: Js_of_ocaml.Js.readonly_prop(tree),
    })
) => {
    let (Node (s, c)) = props##.tree;
    let (collapse, setCollapse) = React.useState(() => s == "context" );
  <li>
    <span onClick={_ => {setCollapse(x => !x)} }
        className={(List.length(c) > 0 ? "caret " : "") ++ (collapse ? "" : "caret-down")}>{s |> React.string}</span>
    {
      if (List.length(c) > 0 && !collapse) {
        <ul>
            {List.mapi( (i, n) => {React.createElement(
              make,
              makeProps(
                ~tree=n, ~key=string_of_int(i),
                (),
              ),
            )}, c) |> React.list}
        </ul>
      } else{
        React.null
      }
    }
  </li>;
};