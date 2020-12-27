open Js_of_ocaml;

let insert_space = i => (i < 10 ? "  " : " ") |> React.string;

[@react.component]
let make =
    (
      ~text,
      ~numb,
      ~dispatch,
      ~hasc: bool,
      ~highlight: bool,
      ~hasDeadCode: bool,
      ~warnings,
    ) => {
  let codeRef = React.useRef(Js.null);
  React.useEffect(() => {
    switch (codeRef |> React.Ref.current |> Js.Opt.to_option) {
    | Some(el) => Js_of_ocaml.Js.Unsafe.global##.Prism##highlightElement(el)
    | None => ()
    };
    None;
  });

  <div className="relative">
    {highlight
       ? <div className="arrow">
           <div className="arrow-carret"> {"►" |> React.string} </div>
         </div>
       : React.null}
    <pre
      className={"lang-C remove-margin " ++ (hasc ? "cursor " : "")}
      style={React.Dom.Style.make(
        ~margin="0",
        ~padding="1px 16px",
        ~borderRadius="0px",
        ~backgroundColor=highlight ? "#E1EDFD" : "",
        (),
      )}
      onClick={_ =>
        if (hasc) {
          dispatch @@ `Set_line(numb);
        }
      }>
      {let line_number = string_of_int(numb) |> React.string;

       if (hasc) {
         <b
           style={React.Dom.Style.make(
             ~color=hasDeadCode ? "#DD4A68" : "black",
             (),
           )}>
           line_number
           {List.length(warnings) > 0
              ? {
                <span className="dot" />;
              }
              : React.null}
           {insert_space(numb)}
         </b>;
       } else {
         <span> line_number {insert_space(numb)} </span>;
       }}
      <code
        style={React.Dom.Style.make(
          ~textShadow=highlight ? "0 0px white" : "",
          ~textDecoration=hasDeadCode && hasc ? "line-through" : "",
          (),
        )}
        ref={React.Dom.Ref.domRef(codeRef)}>
        {text |> React.string}
      </code>
    </pre>
    {if (highlight) {
       warnings
       |> List.map(w =>
            <div className="warning">
              {Parse.warning_to_text(w) |> React.string}
            </div>
          )
       |> React.list;
     } else {
       React.null;
     }}
  </div>;
};
