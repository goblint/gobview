open Batteries;

[@react.component]
let make = (~pdata, ~cil, ~goblint, ~meta, ~warnings) => {
  let (state, dispatch) =
    React.useReducer(
      Reducer.reducer,
      State.make(~pdata, ~cil, ~goblint, ~meta, ~warnings, ()),
    );

  let fetch_file =
    HttpClient.on_get(res => {
      switch (res) {
      | Ok(s) => dispatch @@ `UpdateFileContents(s)
      | _ => ()
      }
    });

  let fetch_dot = (func, file) => {
    let pattern = Js_of_ocaml.Regexp.regexp("/");
    let url =
      "dot/"
      ++ Js_of_ocaml.Regexp.global_replace(pattern, file, "%252F")
      ++ "/"
      ++ func
      ++ ".dot";
    HttpClient.on_get(
      res => {
        switch (res) {
        | Ok(s) => dispatch @@ `UpdateFuncDot(s)
        | _ => ()
        }
      },
      url,
    );
  };

  React.useEffect1(
    () => {
      switch (state.display) {
      | Some(File(f)) when Option.is_none(f.contents) => fetch_file(f.path)
      | Some(Func(f)) when Option.is_none(f.dot) =>
        fetch_dot(f.name, f.file)
      | _ => ()
      };
      None;
    },
    [|state.display|],
  );

  <>
    <GvNavbar display={state.display} dispatch />
    <div className="container-fluid">
      <div className="row">
        <div className="col-3 border-right vh-100 overflow-auto">
          <Sidebar
            active={state.selected_sidebar}
            goblint
            inspect={state.inspect}
            dispatch
          />
        </div>
        <div className="col-9 d-flex flex-column vh-100">
          <Content state dispatch />
          <Panel state dispatch />
        </div>
      </div>
    </div>
  </>;
};
