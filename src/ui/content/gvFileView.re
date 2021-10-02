open Batteries;

let render = element => {
  let div = Js_of_ocaml.Dom_html.createDiv(Js_of_ocaml.Dom_html.document);
  React.Dom.render(element, div);
  div;
};

[@react.component]
let make =
    (
      ~goblint: GvGoblint.solver_state,
      ~file: GvDisplay.file,
      ~line,
      ~dispatch,
    ) => {
  let lines =
    (file.contents |> Option.map_default(String.count_char(_, '\n'), -1)) + 1;
  let contents =
    file.contents
    |> Option.default(
         "// Cannot display this file. It might be still loading or missing.",
       );

  let (content_widgets, view_zones) =
    Enum.range(1, ~until=lines)
    |> Enum.filter_map(i =>
         if (goblint#has_local_analysis((file.path, i))) {
           let widget =
             Monaco.Editor.IContentWidget.make(
               ~get_id=() => string_of_int(i),
               ~get_dom_node=
                 () =>
                   render(
                     <span
                       className="text-muted link-like"
                       style={React.Dom.Style.make(~fontSize="0.75rem", ())}
                       onClick={_ => dispatch(`InspectLine((file.path, i)))}>
                       {"Inspect" |> React.string}
                     </span>,
                   ),
               ~get_position=
                 () => {
                   let position =
                     Monaco.Position.make(i, 1)
                     |> Monaco.IPosition.of_position;
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
               ~after_line_number=i - 1,
               ~dom_node=render(<div />),
               (),
             );
           (widget, zone) |> Option.some;
         } else {
           None;
         }
       )
    |> List.of_enum
    |> List.split;

  let decorations =
    Enum.range(1, ~until=lines)
    |> Enum.filter_map(i =>
         if (goblint#is_dead(~file=file.path, ~line=i)) {
           let options =
             Monaco.Editor.IModelDecorationOptions.make(
               ~inline_class_name="text-decoration-line-through",
               ~is_whole_line=true,
               (),
             );
           let range =
             Monaco.Range.make(i, 0, i, 0) |> Monaco.IRange.of_range;
           Monaco.Editor.IModelDeltaDecoration.make(~options, ~range, ())
           |> Option.some;
         } else {
           None;
         }
       )
    |> List.of_enum;

  let decorations =
    [
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
      |> Option.map_default(x => [x], []),
      decorations,
    ]
    |> List.concat;

  <GvCodeEditor
    value=contents
    read_only=true
    content_widgets
    decorations
    view_zones
  />;
};
