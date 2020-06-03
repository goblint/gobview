[@react.component]
let make = (~statistics) => {
  <>
    {String.split_on_char('\n', statistics)
     |> List.mapi((i, l) => {
          <div key={string_of_int(i)}> {l |> React.string} </div>
        })
     |> React.list}
  </>;
};