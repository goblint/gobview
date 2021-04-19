open Batteries;

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
let make = (~goblint, ~warnings, ~file: GvDisplay.file, ~inspect, ~dispatch) => {
  let inspected_line =
    inspect
    |> Option.map_default(GvInspect.line_num, None)
    |> Option.default(-1);
  <div
    className="d-inline-block min-w-100"
    style={React.Dom.Style.make(~backgroundColor="#f5f2f0", ())}>
    {String.split_on_char('\n', Option.default("", file.contents))
     |> List.mapi((i, l) => {
          let line = GvInspect.Line.make(~file=file.path, ~num=i + 1);
          <CodeLine
            key={string_of_int(i)}
            text=l
            file={file.path}
            numb={i + 1}
            dispatch
            hasc={goblint#has_local_analysis(line)}
            warnings={State.Warning.find_all(line, warnings)}
            hasDeadCode={goblint#is_dead(~file=file.path, ~line=i + 1)}
            highlight={i + 1 == inspected_line}
          />;
        })
     |> React.list}
  </div>;
};
