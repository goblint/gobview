[@react.component]
let make = (~parameters) => {
    <p>{parameters |> React.string}</p>
}