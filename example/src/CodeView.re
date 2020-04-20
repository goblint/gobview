let log = a => Js_of_ocaml.Firebug.console##log(a);
let hasc = (calls, line) => { List.exists(c => { Parse.get_line(c) == string_of_int(line) },calls) };

[@react.component]
let make = (~dispatch, ~calls, ~code) => {
  <>  
    { String.split_on_char  ('\n', code) |> List.mapi( (i, l) => {
        <> 
          <Code key={string_of_int(i)} text=l numb=(i+1) dispatch hasc={hasc(calls,(i+1))} />
        </>;
    })  |> React.list}
    </>;
};

