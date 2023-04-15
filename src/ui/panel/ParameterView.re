[@react.component]
let make = (~parameters) => {
    let on_change = (_) => ();
    let value = parameters;

    <Input value on_change />;
}