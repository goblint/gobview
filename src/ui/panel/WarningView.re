open Batteries;
module Message = GvMessages.Message;
module Severity = Goblint_lib.Messages.Severity;

[@react.component]
let make = (~display: option(State.display), ~warnings, ~dispatch) => {
  // List of all categories
  let categories =
    Stream.from(i =>
      if (i >= Severity.min && i < Severity.max) {
        Severity.of_enum(i);
      } else {
        None;
      }
    )
    |> Stream.to_list;

  // Filter all warnings by category (and memoize to avoid recomputations)
  let by_cat =
    React.useMemo(() =>
      Array.init(List.length(categories), severity =>
        warnings
        |> List.filter(m => {
             Message.severity_to_hash(m) == Severity.min + severity
           })
      )
    );
  let (categories_displayed, set_categories_displayed) =
    React.useState(() => categories);
  let (current_file_only, set_current_file_only) = React.useState(() => true);

  <div className="filebox">
    {if (List.length(warnings) == 0) {
       <h2> {"No warnings found!" |> React.string} </h2>;
     } else {
       <>
         <ul>
           <li>
             {categories
              |> List.map(current_cat =>
                   <Button
                     on_click={_ => {
                       // Toggle current category if the button is pressed (remove / add from category list)
                       set_categories_displayed(categories =>
                         List.exists(n => current_cat == n, categories)
                           ? List.remove(categories, current_cat)
                           : [current_cat, ...categories]
                       )
                     }}
                     class_=[
                       // Add special styling if the category is selected
                       List.exists(
                         c => current_cat == c,
                         categories_displayed,
                       )
                         ? Message.severity_to_bootstrap_class(current_cat) : "",
                       "mr-3",
                       "btn",
                     ]>
                     {current_cat |> Severity.show |> React.string}
                   </Button>
                 )
              |> React.list}
             // Seperate Button for filtering by the file currently selected.
             <Button
               class_=["btn", !current_file_only ? "alert-dark" : ""]
               on_click={_ => {set_current_file_only(Bool.neg)}}>
               {"Current File" |> React.string}
             </Button>
           </li>
         </ul>
         <ul>
           {categories_displayed
            |> List.map(Severity.hash)
            |> List.map(Array.get(by_cat))
            |> List.flatten
            |> List.filter(w =>
                 if (!current_file_only) {
                   true;
                 } else {
                   Option.map_default(
                     fun
                     | GvDisplay.File(f) => {
                         let warning_path =
                           Message.location(w)
                           |> Option.map_default(
                                GvInspect.Line.of_location %> fst,
                                "",
                              );
                         warning_path == f.path;
                       }
                     | _ => true,
                     true,
                     display,
                   );
                 }
               )
            |> List.map(w =>
                 (
                   Message.severity_to_string(w),
                   Message.to_string(w),
                   Message.location(w),
                   Message.message_to_bootstrap_class(w),
                 )
               )
            |> List.mapi((i, (title, text, loc, alert)) => {
                 let onClick =
                   loc
                   |> Option.map((loc, _) =>
                        dispatch @@
                        `InspectLine(GvInspect.Line.of_location(loc))
                      );
                 <li
                   className={"link-like alert " ++ alert}
                   key={string_of_int(i)}
                   ?onClick>
                   <h4 className="alert-heading"> {title |> React.string} </h4>
                   <p> {text |> React.string} </p>
                 </li>;
               })
            |> React.list}
         </ul>
       </>;
     }}
  </div>;
};
