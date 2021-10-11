open Batteries;
open Js_of_ocaml;
open Monaco;

let make_inspect_link = (goblint, file: GvDisplay.file, dispatch, line) =>
  if (goblint#has_local_analysis((file.path, line))) {
    let span = Dom_html.createSpan(Dom_html.document);
    span##.className := "text-muted link-like" |> Js.string;
    span##.style##.fontSize := "0.75rem" |> Js.string;
    span##.textContent := "Inspect" |> Js.string |> Js.Opt.return;
    span##.onclick :=
      Dom.handler(_ => {
        dispatch(`InspectLine((file.path, line)));
        Js._false;
      });

    let widget =
      Monaco.Editor.IContentWidget.make(
        ~get_id=() => string_of_int(line),
        ~get_dom_node=() => span,
        ~get_position=
          () => {
            let position =
              Monaco.Position.make(line, 1) |> Monaco.IPosition.of_position;
            Monaco.Editor.IContentWidgetPosition.make(
              ~position,
              ~preference=[
                Monaco.Editor.ContentWidgetPositionPreference.Above,
              ],
              (),
            );
          },
        ~allow_editor_overflow=true,
        (),
      );

    let zone =
      Monaco.Editor.IViewZone.make(
        ~after_line_number=line - 1,
        ~dom_node=Dom_html.createDiv(Dom_html.document),
        (),
      );

    (widget, zone) |> Option.some;
  } else {
    None;
  };

type state = {
  editor: Editor.IStandaloneCodeEditor.t,
  mutable decorations: list(string),
  mutable content_widgets: list(Editor.IContentWidget.t),
  mutable view_zones: list(string),
};

let update =
    (
      goblint,
      file: GvDisplay.file,
      contents,
      line,
      dispatch,
      {editor, _} as s,
    ) => {
  let lines = String.count_char(contents, '\n') + 1;

  let decorations =
    line
    |> Option.map(((_, n)) =>
         Monaco.Editor.IModelDeltaDecoration.make(
           ~options=
             Monaco.Editor.IModelDecorationOptions.make(
               ~class_name="bg-info",
               ~is_whole_line=true,
               (),
             ),
           ~range=Monaco.Range.make(n, 0, n, 0) |> Monaco.IRange.of_range,
           (),
         )
       )
    |> Option.map_default(x => [x], []);

  let decorations =
    Enum.range(1, ~until=lines)
    |> Enum.filter_map(n =>
         if (goblint#is_dead(~file=file.path, ~line=n)) {
           let options =
             Monaco.Editor.IModelDecorationOptions.make(
               ~inline_class_name="text-decoration-line-through",
               ~is_whole_line=true,
               (),
             );
           let range =
             Monaco.Range.make(n, 0, n, 0) |> Monaco.IRange.of_range;
           Monaco.Editor.IModelDeltaDecoration.make(~options, ~range, ())
           |> Option.some;
         } else {
           None;
         }
       )
    |> List.of_enum
    |> List.append(decorations);

  let (content_widgets, view_zones) =
    Enum.range(1, ~until=lines)
    |> Enum.filter_map(make_inspect_link(goblint, file, dispatch))
    |> List.of_enum
    |> List.split;

  s.content_widgets
  |> List.iter(Editor.IStandaloneCodeEditor.remove_content_widget(editor));
  content_widgets
  |> List.iter(Editor.IStandaloneCodeEditor.add_content_widget(editor));
  s.content_widgets = content_widgets;

  Editor.IStandaloneCodeEditor.change_view_zones(
    editor,
    accessor => {
      s.view_zones
      |> List.iter(Editor.IViewZoneChangeAccessor.remove_zone(accessor));
      s.view_zones =
        view_zones
        |> List.map(Editor.IViewZoneChangeAccessor.add_zone(accessor));
      ();
    },
  );

  s.decorations =
    Editor.IStandaloneCodeEditor.delta_decorations(
      editor,
      s.decorations,
      decorations,
    );
  ();
};

let make_editor = (goblint, file, contents, line, dispatch) => {
  let options =
    Editor.IStandaloneEditorConstructionOptions.make(~read_only=true, ());
  let state = React.useRef(None);
  let on_mount = e =>
    {editor: e, decorations: [], content_widgets: [], view_zones: []}
    |> Option.some
    |> React.Ref.setCurrent(state);

  React.useEffect4(
    () => {
      state
      |> React.Ref.current
      |> Option.may(update(goblint, file, contents, line, dispatch));
      None;
    },
    (goblint, file, line, dispatch),
  );

  <GvEditor value=contents options on_mount />;
};

[@react.component]
let make = (~goblint, ~file: GvDisplay.file, ~line, ~dispatch) =>
  switch (file.contents) {
  | Some(s) => make_editor(goblint, file, s, line, dispatch)
  | _ =>
    "Cannot display this file. It might be still loading or missing."
    |> React.string
  };
