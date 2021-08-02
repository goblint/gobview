module Position : sig
  type t = private Ojs.t

  val make : int -> int -> t [@@js.new "monaco.Position"]

  val column : t -> int

  val line_number : t -> int
end

module Range : sig
  type t = private Ojs.t

  val make : int -> int -> int -> int -> t [@@js.new "monaco.Range"]
end

module IDisposable : sig
  type t = private Ojs.t

  val dispose : t -> unit
end

module IMarkdownString : sig
  type t = private Ojs.t

  val make : value:string -> t [@@js.builder]
end

module Editor : sig
  module ContentWidgetPositionPreference : sig
    type t = Above [@js 1] | Below [@js 2] | Exact [@js 0] [@@js.enum]
  end

  module IContentWidgetPosition : sig
    type t = private Ojs.t

    val make :
      position:Position.t ->
      preference:ContentWidgetPositionPreference.t list ->
      ?range:Range.t ->
      unit ->
      t
      [@@js.builder]
  end

  module IContentWidget : sig
    type t = private Ojs.t

    val make :
      get_dom_node:(unit -> React.Dom.domElement) ->
      get_id:(unit -> string) ->
      get_position:(unit -> IContentWidgetPosition.t) ->
      ?allow_editor_overflow:bool ->
      unit ->
      t
      [@@js.builder]
  end

  module ICursorPositionChangedEvent : sig
    type t = private Ojs.t

    val position : t -> Position.t
  end

  module ITextModel : sig
    type t = private Ojs.t

    val id : t -> string
  end

  val create_model : string -> ?language:string -> unit -> ITextModel.t
    [@@js.global "monaco.editor.createModel"]

  module IViewZone : sig
    type t = private Ojs.t

    val make :
      after_line_number:int -> dom_node:React.Dom.domElement -> ?height_in_lines:float -> unit -> t
      [@@js.builder]
  end

  module IViewZoneChangeAccessor : sig
    type t = private Ojs.t

    val add_zone : t -> IViewZone.t -> string

    val remove_zone : t -> string -> unit
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

    val make : ?options:IModelDecorationOptions.t -> ?range:Range.t -> unit -> t [@@js.builder]
  end

  module IStandaloneCodeEditor : sig
    type t = private Ojs.t

    val on_did_change_cursor_position :
      t -> (ICursorPositionChangedEvent.t -> unit) -> IDisposable.t

    val add_content_widget : t -> IContentWidget.t -> unit

    val change_view_zones : t -> (IViewZoneChangeAccessor.t -> unit) -> unit

    val delta_decorations : t -> string list -> IModelDeltaDecoration.t list -> string list

    val remove_content_widget : t -> IContentWidget.t -> unit

    val set_value : t -> string -> unit [@@js.call]
  end

  module IStandaloneEditorConstructionOptions : sig
    type t = private Ojs.t

    val make : ?model:ITextModel.t -> ?read_only:bool -> unit -> t [@@js.builder]
  end

  val create :
    React.Dom.domElement ->
    ?options:IStandaloneEditorConstructionOptions.t ->
    unit ->
    IStandaloneCodeEditor.t
    [@@js.global "monaco.editor.create"]
end
