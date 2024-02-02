open React.Dom.Dsl.Html;

[@react.component]
let make = (~parameters) => {
    <p>{parameters |> React.string}</p>
}