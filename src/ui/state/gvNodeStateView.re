open Batteries;

let make_single = (ctx, path) => {
  <CollapsibleList collapsed=false override_class=[]>
    <CollapsibleListItem name="Context" override_class=[]>
      ...<GvRepresentationView represent=ctx />
    </CollapsibleListItem>
    <CollapsibleListItem name="Path" override_class=[]>
      ...<GvRepresentationView represent=path />
    </CollapsibleListItem>
  </CollapsibleList>;
};

[@react.component]
let make = (~goblint, ~inspect) =>
  switch (inspect) {
  | None => React.null
  | Some(inspect) =>
    <CollapsibleList collapsed=false style=`Flush>
      ...{goblint#local_analyses(inspect)
       |> List.group(((id, _), (id', _)) => String.compare(id, id'))
       |> List.map(
            fun
            | [(id, (ctx, path))] =>
              <CollapsibleListItem name={"Node: " ++ id}>
                ...{make_single(ctx, path)}
              </CollapsibleListItem>
            | [(id, _), ..._] as group =>
              <CollapsibleListItem name={"Node: " ++ id}>
                ...<CollapsibleList style=`Flush>
                  ...{group
                   |> List.mapi((i, (_, (ctx, path))) =>
                        <CollapsibleListItem
                          name={"Tuple: " ++ string_of_int(i)}>
                          ...{make_single(ctx, path)}
                        </CollapsibleListItem>
                      )}
                </CollapsibleList>
              </CollapsibleListItem>
            | _ => failwith("List.group returned an empty group"),
          )}
    </CollapsibleList>
  };
