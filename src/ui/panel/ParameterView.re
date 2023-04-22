[@react.component]
let make = (~parameters) => {
    let on_change = (_) => (); // not needed, but required...
    let on_submit = (_) => (); // TODO later needed for new exec of params
    let value = parameters; // TODO maybe use |> and maps to transform ''-args into whole string (concat)

    <>
        <div className="d-flex justify-content-between">
            <Button>
                <IconPlay fill={"fill"} />
            </Button>
            <Input value on_change on_submit />
        </div>
        <div className="d-flex align-content-between flex-wrap">{"History" |> React.string}</div>
        <ol className="list-group" style={height=100px;}>
            <li className="list-group-item">
                {"Hello World" |> React.string}
            </li>
            <li className="list-group-item">
                {"Hello World" |> React.string}
            </li>
            <li className="list-group-item">
                {"Hello World" |> React.string}
            </li>
            <li className="list-group-item">
                {"Hello World" |> React.string}
            </li>
        </ol>
    </>;
}