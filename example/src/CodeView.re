/* let code_list = Str.split (Str.regexp("$"), CodeData.c_code); */
let code_list = String.split_on_char  ('\n', CodeData.c_code);

[@react.component]
let make = () => {
  <>
    { code_list |> List.mapi( (i, l) => {
        <> 
          <Code key={string_of_int(i)} text=l numb=i />
        </>;
    })  |> React.list}
    </>;
};

