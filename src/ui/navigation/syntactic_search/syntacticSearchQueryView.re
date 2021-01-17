module S = State.SyntacticSearch;

[@react.component]
let make = (~query_string, ~query, ~query_error, ~dispatch) => {
  let onChange = ev => {
    let v =
      React.Event.Synthetic.target(ev)
      |> Ojs.get(_, "value")
      |> Ojs.string_of_js;
    dispatch @@ `ParseSynSearchQuery(v);
  };

  let onClick = ev => {
    React.Event.Synthetic.preventDefault(ev);
    dispatch @@ `PerformSynSearch;
  };

  let string_of_error = e =>
    Option.map(S.Query.string_of_error, e) |> Option.value(~default="");

  let (target, set_target) = React.useState(() => Ok(CodeQuery.Name_t("")));
  let (typ, set_typ) = React.useState(() => CodeQuery.Var_k);
  let (find, set_find) = React.useState(() => CodeQuery.Uses_f);
  let (structure, set_structure) = React.useState(() => CodeQuery.None_s);

  React.useEffect4(
    () => {
      switch (target) {
      | Ok(t) =>
        {
          CodeQuery.sel: [Name_sel, Location_sel, Type_sel, ID_sel],
          k: typ,
          tar: t,
          f: find,
          str: structure,
          lim: None_c,
        }
        |> CodeQuery.query_to_yojson
        |> Yojson.Safe.to_string
        |> Util.log
      | _ => ()
      };
      None;
    },
    (target, typ, find, structure),
  );

  <>
    <h5 className="card-title"> {"Enter a query" |> React.string} </h5>
    <textarea
      className={
        "form-control" ++ (Option.is_some(query_error) ? " is-invalid" : "")
      }
      rows=10
      value=query_string
      onChange
    />
    <div className="invalid-feedback">
      {"Invalid query: " ++ string_of_error(query_error) |> React.string}
    </div>
    <button
      type_="button"
      className="btn btn-primary mt-3"
      disabled={Option.is_none(query)}
      onClick>
      {"Execute" |> React.string}
    </button>
    <Form on_submit={() => ()}>
      <SyntacticSearchFindBuilder
        value=find
        on_change={v => set_find(_ => v)}
      />
      <SyntacticSearchKindBuilder value=typ on_change={v => set_typ(_ => v)} />
      <SyntacticSearchTargetBuilder
        value=target
        on_change={v => set_target(_ => v)}
      />
      <SyntacticSearchStructureBuilder
        value=structure
        on_change={v => set_structure(_ => v)}
      />
    </Form>
  </>;
};
