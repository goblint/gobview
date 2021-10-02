open Batteries;
open Monaco;

module Js = Js_of_ocaml.Js;
module Dom = React.Dom;
module Options = Editor.IStandaloneEditorConstructionOptions;

[@react.component]
let make =
    (
      ~width=?,
      ~height=?,
      ~value=?,
      ~default_value=?,
      ~language=?,
      ~options=?,
      ~on_mount=?,
    ) => {
  let (width, height, value, default_value, language, options, on_mount) =
    Utils.fix_opt_args7(
      width,
      height,
      value,
      default_value,
      language,
      options,
      on_mount,
    );
  let width = Option.default("100%", width);
  let height = Option.default("100%", height);
  let default_value = Option.default("", default_value);
  let value = Option.default("", value);
  let language = Option.default("c", language);
  let options = Option.default(Options.make(), options);
  let on_mount = Option.default(_ => (), on_mount);

  let ref = React.useRef(None);

  let dom_ref =
    Dom.Ref.callbackDomRef(
      Js.Opt.iter(_, dom =>
        if (ref |> React.Ref.current |> Option.is_none) {
          let model = Editor.create_model(default_value, ~language, ());
          Options.set_model(options, model);
          let editor = Editor.create(dom, ~options, ());
          editor |> Option.some |> React.Ref.setCurrent(ref);
          on_mount(editor);
          ();
        }
      ),
    );

  React.useEffect1(
    () => {
      let update = editor =>
        Editor.IStandaloneCodeEditor.set_value(editor, value);
      ref |> React.Ref.current |> Option.may(update);
      None;
    },
    [|value|],
  );

  <div ref=dom_ref style={Dom.Style.make(~width, ~height, ())} />;
};
