open Batteries;

let make_single = (ctx, path) => {
  <CollapsibleList style=`Flush>
    <CollapsibleListItem name="Context">
      <GvRepresentationView represent=ctx />
    </CollapsibleListItem>
    <CollapsibleListItem name="Path">
      <GvRepresentationView represent=path />
    </CollapsibleListItem>
  </CollapsibleList>;
};

[@react.component]
let make = (~goblint, ~inspect, ~file_path, ~line, ~id) => {
  let local =
    inspect
    |> Option.map(
         fun
         | InspectState.Graph(_) => `Node(string_of_int(id))
         | InspectState.File(_) => `Line((file_path, line)),
       );

  switch (local) {
  | None => React.null
  | Some(local) =>
    <CollapsibleList style=`Flush>
      {goblint#local_analyses(local)
       |> List.group(((id, _), (id', _)) => String.compare(id, id'))
       |> List.map(
            fun
            | [(id, (ctx, path))] =>
              <CollapsibleListItem key=id name={"Node: " ++ id}>
                {make_single(ctx, path)}
              </CollapsibleListItem>
            | [(id, _), ..._] as group =>
              <CollapsibleListItem key=id name={"Node: " ++ id}>
                <CollapsibleList style=`Flush>
                  {group
                   |> List.mapi((i, (_, (ctx, path))) =>
                        <CollapsibleListItem
                          key={string_of_int(i)}
                          name={"Tuple: " ++ string_of_int(i)}>
                          {make_single(ctx, path)}
                        </CollapsibleListItem>
                      )
                   |> React.list}
                </CollapsibleList>
              </CollapsibleListItem>
            | _ => failwith("BatList.group should never return empty groups"),
          )
       |> React.list}
    </CollapsibleList>
  };
};
