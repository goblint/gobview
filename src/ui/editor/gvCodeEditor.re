open Batteries;

module Dom = React.Dom;
module Js = Js_of_ocaml.Js;

module Editor = Monaco.Editor;
module IStandaloneCodeEditor = Editor.IStandaloneCodeEditor;
module IViewZoneChangeAccessor = Editor.IViewZoneChangeAccessor;
module IViewZone = Editor.IViewZone;

module ViewZone = {
  type t = {
    element: React.element,
    after_line_number: int,
    height_in_lines: option(float),
  };

  let make = (~height_in_lines=?, element, after_line_number) => {
    element,
    after_line_number,
    height_in_lines,
  };
};

type state = {
  editor: IStandaloneCodeEditor.t,
  mutable view_zones: list(string),
};

[@react.component]
let make = (~value=?, ~read_only=?, ~view_zones=?) => {
  let (value, read_only, view_zones) =
    Utils.fix_opt_args3(value, read_only, view_zones);
  let value = Option.default("", value);
  let read_only = Option.default(false, read_only);
  let view_zones = Option.default([], view_zones);

  let update = (aspect, {editor, _} as s) =>
    switch (aspect) {
    | `Value => IStandaloneCodeEditor.set_value(editor, value)
    | `ViewZones =>
      IStandaloneCodeEditor.change_view_zones(
        editor,
        accessor => {
          List.iter(
            IViewZoneChangeAccessor.remove_zone(accessor),
            s.view_zones,
          );
          s.view_zones =
            view_zones
            |> List.map(
                 ({element, after_line_number, height_in_lines}: ViewZone.t) => {
                 let dom_node =
                   Js_of_ocaml.Dom_html.createDiv(
                     Js_of_ocaml.Dom_html.document,
                   );
                 ignore(Dom.render(element, dom_node));
                 IViewZoneChangeAccessor.add_zone(
                   accessor,
                   IViewZone.make(
                     ~after_line_number,
                     ~dom_node,
                     ~height_in_lines?,
                   ),
                 );
               });
          ();
        },
      )
    };

  let state = React.useRef(None);

  let ref =
    Dom.Ref.callbackDomRef(
      Js.Opt.iter(_, dom_element =>
        if (state |> React.Ref.current |> Option.is_none) {
          let model = Editor.create_model(~value="", ~language="c", ());
          let options =
            Editor.IStandaloneEditorConstructionOptions.make(
              ~model,
              ~read_only,
              (),
            );
          let editor = Editor.create(~dom_element, ~options, ());
          React.Ref.setCurrent(state, Some({editor, view_zones: []}));
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
      state |> React.Ref.current |> Option.may(update(`ViewZones));
      None;
    },
    [|view_zones|],
  );

  <div ref style={Dom.Style.make(~height="600px", ())} />;
};
