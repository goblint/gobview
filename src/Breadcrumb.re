open State;

[@react.component]
let make = (~state, ~dispatch) => {
  switch (state.inspect) {
  | Some(Function(f)) =>
    <>
      <nav>
        <ol className="breadcrumb">
          <li className="breadcrumb-item">
            <a href="#"> {"Analysis" |> React.string} </a>
          </li>
          <li className="breadcrumb-item">
            <a href="#"> {f.file_name |> React.string} </a>
          </li>
          <li className="breadcrumb-item active">
            {f.name |> React.string}
          </li>
        </ol>
      </nav>
    </>
  | _ => React.null
  };
};
