open Batteries;

[@react.component]
let make = (~kind, ~target, ~find, ~structure, ~expression, ~mode, ~dispatch) => {
  let on_change_kind = k => dispatch @@ `UpdateSearchField(`Kind(k));
  let on_change_target = t => dispatch @@ `UpdateSearchField(`Target(t));
  let on_change_find = f => dispatch @@ `UpdateSearchField(`Find(f));
  let on_change_structure = s =>
    dispatch @@ `UpdateSearchField(`Structure(s));
  let on_change_expression = e => if(String.trim(e) != ""){dispatch @@ `UpdateSearchField(`Expression(Some(e)))} else ();
  let on_change_mode = m => dispatch @@ `UpdateSearchField(`Mode(m));

  let on_click = () => dispatch @@ `StartPerformSearch;

  <>
    <SearchKindBuilder value=kind on_change=on_change_kind />
    <SearchTargetBuilder value=target on_change=on_change_target />
    <SearchFindBuilder value=find on_change=on_change_find />
    <SearchStructureBuilder value=structure on_change=on_change_structure />
    <SearchExpressionBuilder value={Option.default("",expression)} on_change=on_change_expression />
    <SearchModeBuilder value=mode on_change=on_change_mode />
    <Button type_=`Button on_click disabled={Result.is_error(target)}>
      {"Execute" |> React.string}
    </Button>
  </>;
};
