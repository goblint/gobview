module MarkerSeverity : sig
  type t = Hint [@js 1] | Info [@js 2] | Warning [@js 4] | Error [@js 8] [@@js.enum]
end

module Position : sig
  type t = private Ojs.t

  val make : int -> int -> t [@@js.new "monaco.Position"]

  val column : t -> int [@@js.get]

  val line_number : t -> int [@@js.get]
end

module Range : sig
  type t = private Ojs.t

  val make : int -> int -> int -> int -> t [@@js.new "monaco.Range"]
end

module IDisposable : sig
  type t = private Ojs.t

  val dispose : t -> unit [@@js.call]
end

module IMarkdownString : sig
  type t = private Ojs.t

  val make : value:string -> unit -> t [@@js.builder]
end

module IPosition : sig
  type t = private Ojs.t

  val of_position : Position.t -> t [@@js.cast]
end

module IRange : sig
  type t = private Ojs.t

  val of_range : Range.t -> t [@@js.cast]
end

module Editor : sig
  module ContentWidgetPositionPreference : sig
    type t = Exact [@js 0] | Above [@js 1] | Below [@js 2] [@@js.enum]
  end

  module IContentWidgetPosition : sig
    type t = private Ojs.t

    val make :
      position:IPosition.t ->
      preference:ContentWidgetPositionPreference.t list ->
      ?range:IRange.t ->
      unit ->
      t
      [@@js.builder]
  end

  module IContentWidget : sig
    type t = private Ojs.t

    val make :
      ?allow_editor_overflow:bool ->
      get_dom_node:(unit -> React.Dom.dom_element) ->
      get_id:(unit -> string) ->
      get_position:(unit -> IContentWidgetPosition.t) ->
      unit ->
      t
      [@@js.builder]
  end

  module ICursorPositionChangedEvent : sig
    type t = private Ojs.t

    val position : t -> Position.t [@@js.get]
  end

  module IMarkerData : sig
    type t = private Ojs.t

    val make :
      end_column:int ->
      end_line_number:int ->
      message:string ->
      severity:MarkerSeverity.t ->
      start_column:int ->
      start_line_number:int ->
      unit ->
      t
      [@@js.builder]
  end

  module IModelDecorationOptions : sig
    type t = private Ojs.t

    val make :
      ?class_name:string ->
      ?hover_message:IMarkdownString.t ->
      ?inline_class_name:string ->
      ?is_whole_line:bool ->
      unit ->
      t
      [@@js.builder]
  end

  module IModelDeltaDecoration : sig
    type t = private Ojs.t

    val make : options:IModelDecorationOptions.t -> range:IRange.t -> unit -> t [@@js.builder]
  end

  module IViewZone : sig
    type t = private Ojs.t

    val make :
      after_line_number:int -> dom_node:React.Dom.dom_element -> ?height_in_lines:float -> unit -> t
      [@@js.builder]
  end

  module IViewZoneChangeAccessor : sig
    type t = private Ojs.t

    val add_zone : t -> IViewZone.t -> string [@@js.call]

    val remove_zone : t -> string -> unit [@@js.call]
  end

  module ITextModel : sig
    type t = private Ojs.t

    val id : t -> string [@@js.get]
  end

  module IStandaloneCodeEditor : sig
    type t = private Ojs.t

    val on_did_change_cursor_position :
      t -> (ICursorPositionChangedEvent.t -> unit) -> IDisposable.t
      [@@js.call]

    val add_content_widget : t -> IContentWidget.t -> unit [@@js.call]

    val change_view_zones : t -> (IViewZoneChangeAccessor.t -> unit) -> unit [@@js.call]

    val delta_decorations : t -> string list -> IModelDeltaDecoration.t list -> string list
      [@@js.call]

    val remove_content_widget : t -> IContentWidget.t -> unit [@@js.call]

    val get_model : t -> ITextModel.t [@@js.call]

    val set_value : t -> string -> unit [@@js.call]
  end

  module IStandaloneEditorConstructionOptions : sig
    type t = private Ojs.t

    val make :
      ?model:ITextModel.t -> ?read_only:bool -> ?render_validation_decorations:string -> unit -> t
      [@@js.builder]

    val set_model : t -> ITextModel.t -> unit [@@js.set]
  end

  val create :
    React.Dom.dom_element ->
    ?options:IStandaloneEditorConstructionOptions.t ->
    unit ->
    IStandaloneCodeEditor.t
    [@@js.global "monaco.editor.create"]

  val create_model : string -> ?language:string -> unit -> ITextModel.t
    [@@js.global "monaco.editor.createModel"]

  val set_model_markers : ITextModel.t -> string -> IMarkerData.t list -> unit
    [@@js.global "monaco.editor.setModelMarkers"]
end
