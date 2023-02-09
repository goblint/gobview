open Batteries;

[@react.component]
let make = (~cil, ~goblint, ~warnings, ~meta, ~stats, ~file_loc) => {
  let (state, dispatch) =
    React.useReducer(
      Reducer.reducer,
      State.make(~cil, ~goblint, ~warnings, ~meta, ~stats, ~file_loc, ()),
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
      | Some(File(f)) when Option.is_none(f.contents) => fetch_file(Hashtbl.find(file_loc, f.path))
      | Some(Func(f)) when Option.is_none(f.dot) =>
        fetch_dot(f.name, f.file)
      | _ => ()
      };
      None;
    },
    [|state.display|],
  );

  <div className="container-fluid">
    <div className="row">
      <div className="col-2 border-end vh-100">
        <SidebarLeft
          active={state.selected_sidebar_left}
          dispatch
          search={state.search}
          cil={state.cil}
        />
      </div>
      <div className="col-8 d-flex flex-column vh-100 p-0">
        {switch (state.display) {
        | None => <div className="d-flex flex-column h-75 overflow-auto p-2" />
        | Some(f) => <Content state display=f dispatch />
        }}
        <Panel state dispatch />
      </div>
      <div className="col-2 border-start overflow-auto vh-100">
          <SidebarRight
            active={state.selected_sidebar_right}
            goblint
            inspect={state.inspect}
            dispatch
          />
      </div>
    </div>
  </div>;
};
