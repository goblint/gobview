open React.Dom.Dsl.Html;
open Batteries;
open GoblintCil;

[@react.component]
let make = (~cil, ~goblint, ~warnings, ~meta, ~stats, ~file_loc) => {
  let mainFiles = {
    let (mf,_,_) = Goblint_lib.Cilfacade.getFuns(cil);
    Cil.foldGlobals(
      cil,
      (mainFiles, g) =>
      switch(g) {
      | GFun(fdec, loc) =>
        if(List.mem(fdec, mf)) {
          [loc.file, ...mainFiles]
        } else {
          mainFiles
        };
      | _ => mainFiles
      },
      [],
    )};

  let (state, dispatch) = {
    let file = GvDisplay.File.make(~path=List.hd(mainFiles), ());
    React.use_reducer(
      Reducer.reducer,
      ~init=fun () => State.make(~file, ~cil, ~goblint, ~warnings, ~meta, ~stats, ~file_loc, ()),
    )};

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

  React.use_effect1(
    () => {
      switch (state.display) {
      | Some(File(f)) when Option.is_none(f.contents) =>
        try (fetch_file(Hashtbl.find(file_loc, f.path))) {
          | Not_found => ()
        };
      | Some(Func(f)) when Option.is_none(f.dot) =>
        fetch_dot(f.name, f.file)
      | Some(Cil(f)) when Option.is_none(f.contents) =>
        fetch_file(f.path)
      | _ => ()
      };
      None;
    },
    [|state.display|],
  );

  <div className="container-fluid">
  <header className="navbar navbar-dark fixed-top bg-dark flex-md-nowrap shadow p-1">
    <div className="navbar-brand mx-2" >{React.string("Gobview")}</div>
  </header>
  <div className="main row vh-100">
    <div className="col-3 border-end overflow-auto bg-light py-2 h-100">
      <SidebarLeft
        active={state.selected_sidebar_left}
        dispatch
        search={state.search}
        cil={state.cil}
        mainFiles
      />
    </div>
    <div className="col-6 d-flex flex-column h-100">
      {switch (state.display) {
      | None => <div className="content d-flex flex-column h-75 overflow-auto p-4" />
      | Some(f) => <Content state display=f dispatch />
      }}
      <Panel state dispatch />
    </div>
    <div className="col-3 border-start overflow-auto py-2 h-100">
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
