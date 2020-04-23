let log = a => Js_of_ocaml.Firebug.console##log(a);

[@react.component]
let make = (~text, ~numb, ~dispatch, ~hasc:bool, ~highlight:bool) => {
  let codeRef = React.useRef(None);
  React.useEffect(() => {
    switch (codeRef |> React.Ref.current) {
    | Some(el) => Js_of_ocaml.Js.Unsafe.global##.Prism##highlightElement(el)
    | None => ()
    };
    None;
  });
  
  <div className="relative">
    {highlight ? <div className="arrow"><div className="arrow-carret">{"►" |> React.string}</div></div> : React.null}
    <pre className={"lang-C remove-margin " ++ (hasc ? "cursor " : "")} 
      style={ReactDOM.Style.make(~margin="0",~padding="1px 16px",~borderRadius="0px",~backgroundColor=(highlight ? "#E1EDFD" : ""), ())}
      onClick={_=>{ if (hasc) { dispatch((_:int) => numb ) }}}>
      
      {let line_number =  string_of_int(numb)++"  " ++ (numb < 10 ? " " : "") |> React.string;
        hasc ? <b>line_number</b> : line_number;
      }
      
      <code style={ReactDOM.Style.make( ~textShadow=(highlight ? "0 0px white" : ""),())} ref={ReactDOM.Ref.domRef(codeRef)}> {text |> React.string} </code>
    </pre>
  </div>
};
