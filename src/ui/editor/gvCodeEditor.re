open Batteries;

module Dom = React.Dom;
module Js = Js_of_ocaml.Js;

module Editor = Monaco.Editor;
module IStandaloneCodeEditor = Editor.IStandaloneCodeEditor;
module IViewZoneChangeAccessor = Editor.IViewZoneChangeAccessor;

type state = {
  editor: IStandaloneCodeEditor.t,
  mutable content_widgets: list(Editor.IContentWidget.t),
  mutable decorations: list(string),
  mutable view_zones: list(string),
  mutable on_did_change_cursor_position: option(Monaco.IDisposable.t),
};

[@react.component]
let make =
    (
      ~value=?,
      ~read_only=?,
      ~content_widgets=?,
      ~decorations=?,
      ~view_zones=?,
      ~on_did_change_cursor_position=?,
    ) => {
  let (
    value,
    read_only,
    content_widgets,
    decorations,
    view_zones,
    on_did_change_cursor_position,
  ) =
    Utils.fix_opt_args6(
      value,
      read_only,
      content_widgets,
      decorations,
      view_zones,
      on_did_change_cursor_position,
    );
  let value = Option.default("", value);
  let read_only = Option.default(false, read_only);
  let content_widgets = Option.default([], content_widgets);
  let decorations = Option.default([], decorations);
  let view_zones = Option.default([], view_zones);

  let update = (aspect, {editor, _} as s) =>
    switch (aspect) {
    | `Value => IStandaloneCodeEditor.set_value(editor, value)
    | `ContentWidgets =>
      s.content_widgets
      |> List.iter(IStandaloneCodeEditor.remove_content_widget(editor));
      content_widgets
      |> List.iter(IStandaloneCodeEditor.add_content_widget(editor));
      s.content_widgets = content_widgets;
    | `Decorations =>
      s.decorations =
        IStandaloneCodeEditor.delta_decorations(
          editor,
          s.decorations,
          decorations,
        )
    | `ViewZones =>
      IStandaloneCodeEditor.change_view_zones(
        editor,
        accessor => {
          s.view_zones
          |> List.iter(IViewZoneChangeAccessor.remove_zone(accessor));
          s.view_zones =
            view_zones
            |> List.map(IViewZoneChangeAccessor.add_zone(accessor));
          ();
        },
      )
    | `OnDidChangeCursorPosition =>
      s.on_did_change_cursor_position
      |> Option.may(Monaco.IDisposable.dispose);
      s.on_did_change_cursor_position =
        on_did_change_cursor_position
        |> Option.map(
             IStandaloneCodeEditor.on_did_change_cursor_position(editor),
           );
    };

  let state = React.useRef(None);

  let ref =
    Dom.Ref.callbackDomRef(
      Js.Opt.iter(_, dom =>
        if (state |> React.Ref.current |> Option.is_none) {
          let model = Editor.create_model("", ~language="c", ());
          let options =
            Editor.IStandaloneEditorConstructionOptions.make(
              ~model,
              ~read_only,
              (),
            );
          let editor = Editor.create(dom, ~options, ());
          {
            editor,
            content_widgets: [],
            decorations: [],
            view_zones: [],
            on_did_change_cursor_position: None,
          }
          |> Option.some
          |> React.Ref.setCurrent(state);
        }
      ),
    );

  React.useEffect1(
    () => {
      state |> React.Ref.current |> Option.may(update(`Value));
      None;
    },
    [|value|],
  );

  React.useEffect1(
    () => {
      state |> React.Ref.current |> Option.may(update(`ContentWidgets));
      None;
    },
    [|content_widgets|],
  );

  React.useEffect1(
    () => {
      state |> React.Ref.current |> Option.may(update(`Decorations));
      None;
    },
    [|decorations|],
  );

  React.useEffect1(
    () => {
      state |> React.Ref.current |> Option.may(update(`ViewZones));
      None;
    },
    [|view_zones|],
  );

  React.useEffect1(
    () => {
      state
      |> React.Ref.current
      |> Option.may(update(`OnDidChangeCursorPosition));
      None;
    },
    [|on_did_change_cursor_position|],
  );

  <div ref style={Dom.Style.make(~height="600px", ())} />;
};
