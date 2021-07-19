open Batteries;

module Dom = React.Dom;
module Js = Js_of_ocaml.Js;

module Editor = Monaco.Editor;
module IStandaloneCodeEditor = Editor.IStandaloneCodeEditor;

[@react.component]
let make = (~value=?, ~read_only=?) => {
  let (value, read_only) = Utils.fix_opt_args2(value, read_only);
  let value = Option.default("", value);
  let read_only = Option.default(false, read_only);

  let editor = React.useRef(None);

  React.useEffect1(
    () => {
      editor
      |> React.Ref.current
      |> Option.may(e => IStandaloneCodeEditor.set_value(e, value));
      None;
    },
    [|value|],
  );

  let ref =
    Dom.Ref.callbackDomRef(
      Js.Opt.iter(_, dom_element =>
        if (editor |> React.Ref.current |> Option.is_none) {
          let model = Editor.create_model(~value, ~language="c", ());
          Editor.create(~dom_element, ~options={model, read_only}, ())
          |> Option.some
          |> React.Ref.setCurrent(editor);
        }
      ),
    );

  <div ref style={Dom.Style.make(~height="600px", ())} />;
};
