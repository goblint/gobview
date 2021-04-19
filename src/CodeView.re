open State;

let hasc = (calls, line, filepath) => {
  List.exists(
    c => {
      Parse.get_line(c) == string_of_int(line)
      && String.equal(Parse.get_file(c), filepath)
    },
    calls,
  );
};
let has_dead_code = (calls, line, filepath) => {
  !
    List.exists(
      c => {
        Parse.get_line(c) == string_of_int(line)
        && String.equal(Parse.get_file(c), filepath)
        && !Parse.has_dead_code(c)
      },
      calls,
    );
};
let get_all_warnings = (warnings, line, filepath) => {
  List.find_all(
    w => {
      String.equal(Parse.warning_to_file(w), filepath)
      && String.equal(Parse.warning_to_line(w), string_of_int(line))
    },
    warnings,
  );
};
[@react.component]
let make = (~state, ~code, ~file, ~dispatch, ~calls, ~warnings) => {
  <div
    className="d-inline-block min-w-100"
    style={React.Dom.Style.make(~backgroundColor="#f5f2f0", ())}>
    {String.split_on_char('\n', code)
     |> List.mapi((i, l) => {
          <CodeLine
            key={string_of_int(i)}
            text=l
            numb={i + 1}
            dispatch
            hasc={
              (state.goblint)#has_local_analysis(
                GvInspect.Line.make(~file, ~num=i + 1),
              )
            }
            warnings={get_all_warnings(warnings, i + 1, file)}
            hasDeadCode={(state.goblint)#is_dead(~line=i + 1, ~file)}
            highlight={i + 1 == state.line}
          />
        })
     |> React.list}
  </div>;
};
