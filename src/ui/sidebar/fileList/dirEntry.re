open Batteries;

[@react.component]
let make = (~name, ~children, ~collapsed) => {
  <CollapsibleList collapsed=collapsed>
    <CollapsibleListItem name=name>
      children
    </CollapsibleListItem>
  </CollapsibleList>;
};
