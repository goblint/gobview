let hasc = (calls, line, filepath) => { List.exists(c => { Parse.get_line(c) == string_of_int(line) && String.equal(Parse.get_file(c), filepath) },calls)  };
let has_dead_code = (calls, line, filepath) => { 
  !List.exists(c => { Parse.get_line(c) == string_of_int(line) 
    && String.equal(Parse.get_file(c), filepath)
    && !Parse.has_dead_code(c)
  },calls)}; 
let get_all_warnings = (warnings, line, filepath) => 
  {List.find_all(w => {String.equal(Parse.warning_to_file(w), filepath) && String.equal(Parse.warning_to_line(w), string_of_int(line)) } ,warnings)};
[@react.component]
let make = (~dispatch, ~calls, ~code, ~line, ~filepath:string, ~warnings) => {
  <>  
    { String.split_on_char  ('\n', code) |> List.mapi( (i, l) => {
          <Code key={string_of_int(i)} text=l numb=(i+1) dispatch hasc={hasc(calls,(i+1), filepath)} 
            warnings={get_all_warnings(warnings, i, filepath)}
            hasDeadCode={has_dead_code(calls,(i+1), filepath)} highlight={(i+1) == line } />
    })  |> React.list}
    </>;
};

