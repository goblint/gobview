open React.Dom.Dsl.Html;

[@react.component]
let make = (~children) => {
  <div className="invalid-feedback"> children </div>;
};
