let log = a => Js_of_ocaml.Firebug.console##log(a);

[@react.component]
let make = (~text, ~numb, ~dispatch) => {
  let codeRef = React.useRef(None);
  React.useEffect(() => {
    switch (codeRef |> React.Ref.current) {
    | Some(el) => Js_of_ocaml.Js.Unsafe.global##.Prism##highlightElement(el)
    | None => ()
    };
    None;
  });
  <pre className="lang-C remove-margin" style={ReactDOM.Style.make(~margin="0",~padding="1px 16px",~borderRadius="0px", ())}
    onClick={_=>{ log(Js_of_ocaml.Js.string(string_of_int(numb))); dispatch(x => {let x = x-x; numb+x }); } }>
    { string_of_int(numb)++" " |> React.string }
    <code ref={ReactDOM.Ref.domRef(codeRef)}> {text |> React.string} </code>
  </pre>;
};
