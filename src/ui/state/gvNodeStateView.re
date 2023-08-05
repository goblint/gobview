open Batteries;

let make_single = (ctx, path, dispatch) => {
  <CollapsibleList collapsed=false override_class=[]>
    <CollapsibleListItem name="Context" override_class=[]>
    <button
      className="btn btn-sm btn-secondary mb-4"
      onClick={_ => {dispatch @@ `DisplayUsageGraph(Some(ctx))}}>
      {"Find Usages in this Context" |> React.string}
    </button>
      <GvRepresentationView represent=ctx />
    </CollapsibleListItem>
    <CollapsibleListItem name="Path" override_class=[]>
    <button
      className="btn btn-sm btn-secondary mb-4"
      onClick={_ => {dispatch @@ `DisplayUsageGraph(Some(ctx))}}>
      {"Find Usages in this Context" |> React.string}
    </button>
      <GvRepresentationView represent=path />
    </CollapsibleListItem>
  </CollapsibleList>;
};

[@react.component]
let make = (~goblint, ~inspect, ~dispatch) =>
  switch (inspect) {
  | None => React.null
  | Some(inspect) =>
    <CollapsibleList collapsed=false style=`Flush>
      {goblint#local_analyses(inspect)
       |> List.group(((id, _), (id', _)) => String.compare(id, id'))
       |> List.map(
            fun
            | [(id, (ctx, path))] =>
              <CollapsibleListItem name={"Node: " ++ id}>
                {make_single(ctx, path, dispatch)}
              </CollapsibleListItem>
            | [(id, _), ..._] as group =>
              <CollapsibleListItem name={"Node: " ++ id}>
                <CollapsibleList style=`Flush>
                  {group
                   |> List.mapi((i, (_, (ctx, path))) =>
                        <CollapsibleListItem
                          name={"Tuple: " ++ string_of_int(i)}>
                          {make_single(ctx, path, dispatch)}
                        </CollapsibleListItem>
                      )
                   |> React.list}
                </CollapsibleList>
              </CollapsibleListItem>
            | _ => failwith("List.group returned an empty group"),
          )
       |> React.list}
    </CollapsibleList>
  };
