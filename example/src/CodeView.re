let hasc = (calls, line, filepath) => { List.exists(c => { Parse.get_line(c) == string_of_int(line) && String.equal(Parse.get_file(c), filepath) },calls)  };
let has_dead_code = (calls, line, filepath) => { 
  !List.exists(c => { Parse.get_line(c) == string_of_int(line) 
    && String.equal(Parse.get_file(c), filepath)
    && !Parse.has_dead_code(c)
  },calls)}; 
[@react.component]
let make = (~dispatch, ~calls, ~code, ~line, ~filepath:string) => {
  <>  
    { String.split_on_char  ('\n', code) |> List.mapi( (i, l) => {
        <> 
          <Code key={string_of_int(i)} text=l numb=(i+1) dispatch hasc={hasc(calls,(i+1), filepath)} hasDeadCode={has_dead_code(calls,(i+1), filepath)} highlight={(i+1) == line } />
        </>;
    })  |> React.list}
    </>;
};

