[@react.component]
let make = (~kind, ~target, ~find, ~structure, ~dispatch) => {
  let on_submit = () => dispatch @@ `PerformSynSearch;

  let on_change_kind = k => {
    dispatch @@ `UpdateSynSearchField(`Kind(k));
  };

  let on_change_target = t => {
    dispatch @@ `UpdateSynSearchField(`Target(t));
  };

  let on_change_find = f => {
    dispatch @@ `UpdateSynSearchField(`Find(f));
  };

  let on_change_structure = s => {
    dispatch @@ `UpdateSynSearchField(`Structure(s));
  };

  <Form on_submit>
    <SyntacticSearchKindBuilder value=kind on_change=on_change_kind />
    <SyntacticSearchTargetBuilder value=target on_change=on_change_target />
    <SyntacticSearchFindBuilder value=find on_change=on_change_find />
    <SyntacticSearchStructureBuilder
      value=structure
      on_change=on_change_structure
    />
    <Button type_=`Submit disabled={Result.is_error(target)}>
      {"Execute" |> React.string}
    </Button>
  </Form>;
};
