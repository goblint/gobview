open Batteries;

[@react.component]
let make = (~analyses) => {
  <CollapsibleList style=`Flush>
    ...{analyses
     |> List.map(((n, results)) => {
          <CollapsibleListItem name=n>
            ...<GvAnalysesView results />
          </CollapsibleListItem>
        })}
  </CollapsibleList>;
};
