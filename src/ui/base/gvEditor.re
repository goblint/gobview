open React.Dom.Dsl.Html;
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
  let width = Option.default("100%", width);
  let height = Option.default("100%", height);
  let default_value = Option.default("", default_value);
  let language = Option.default("c", language);
  let options = Option.default(Options.make(), options);
  let on_mount = Option.default(_ => (), on_mount);

  let ref = React.use_ref(None);

  let dom_ref =
    Dom.Ref.callback_dom_ref(
      Js.Opt.iter(_, dom =>
        if (ref |> React.Ref.current |> Option.is_none) {
          let model = Editor.create_model(default_value, ~language, ());
          Options.set_model(options, model);
          let editor = Editor.create(dom, ~options, ());
          editor |> Option.some |> React.Ref.set_current(ref);
          on_mount(editor);
          ();
        }
      ),
    );

  React.use_effect1(
    () => {
      switch (value, ref |> React.Ref.current) {
      | (Some(v), Some(r)) => Editor.IStandaloneCodeEditor.set_value(r, v)
      | _ => ()
      };
      None;
    },
    [|value|],
  );

  <div ref_=dom_ref style=Dom.Style.make([|Dom.Style.width(width), Dom.Style.height(height)|]) />;
};
