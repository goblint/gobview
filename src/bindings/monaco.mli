module Editor : sig
  module ITextModel : sig
    type t = private Ojs.t

    val id : t -> string
  end

  val create_model : value:string -> ?language:string -> unit -> ITextModel.t
    [@@js.global "monaco.editor.createModel"]

  module IStandaloneCodeEditor : sig
    type t = private Ojs.t

    val set_value : t -> string -> unit [@@js.call]
  end

  module IStandaloneEditorConstructionOptions : sig
    type t = { model : ITextModel.t; read_only : bool }
  end

  val create :
    dom_element:React.Dom.domElement ->
    ?options:IStandaloneEditorConstructionOptions.t ->
    unit ->
    IStandaloneCodeEditor.t
    [@@js.global "monaco.editor.create"]
end
