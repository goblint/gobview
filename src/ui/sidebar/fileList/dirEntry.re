open Batteries;

[@react.component]
let make = (~name, ~children ) => {
  <CollapsibleList>
    <CollapsibleListItem name=name>
      children
    </CollapsibleListItem>
  </CollapsibleList>;
};
