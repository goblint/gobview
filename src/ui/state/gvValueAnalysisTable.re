open Batteries;

type value =
  | Exact(string, int)
  | Interval(string, int, int)
  | Exclusion(string, int, int, list(int));

let rec find_node = (represent: Representation.t, name) =>
  switch (represent) {
  | `Tagged(n, _) when n == name => Some(represent)
  | `Tagged(_, child) => find_node(child, name)
  | `Assoc(list) =>
    // Convert Association list to multiple `Tagged then search them
    list |> List.find_map_opt(((n, v)) => find_node(`Tagged((n, v)), name))
  | `Pair(a, b) => [a, b] |> List.find_map_opt(find_node(_, name))
  | `List(list) => List.find_map_opt(find_node(_, name), list)
  | `Value(_) => None
  | `Nothing => None
  };

let var_eq_to_values = (var_eq: Representation.t) => {
  let equalities_to_entries = l =>
    switch (l) {
    | `List(equalities) =>
      equalities
      |> List.find_map_opt(
           fun
           // find int values
           | `Value(v) => v |> int_of_string_opt
           | _ => None,
         )
      |> Option.map_default(
           value =>
             equalities
             |> List.filter_map(
                  fun
                  | `Value(name) when name != string_of_int(value) =>
                    Some(name)
                  | _ => None,
                )
             |> List.map(name => Exact(name, value)),
           [],
         )
    | _ => []
    };

  switch (var_eq) {
  | `List(l) => l |> List.map(equalities_to_entries) |> List.concat
  | _ => []
  };
};
let value_domain_to_entries = (interval: Representation.t) => {
  let expressions_to_entries =
    fun
    | (name, vals) => {
        switch (vals) {
        | `List(value_list) =>
          value_list
          |> List.filter_map(
               fun
               | `Value(v) => {
                   // Equivalent to "\[[-0-9]+,[-0-9+]\]"
                   let interval_regex = Str.regexp("\\[\\(.+\\),\\(.+\\)\\]");

                   // Equivalent to "Not {[-0-9]+}\([-0-9]+,[-0-9]+\)"
                   let exc_regex =
                     Str.regexp(
                       "Not {\\(.+\\)}(\\[\\([-0-9]+\\),\\([-0-9]+\\)\\])",
                     );
                   if (Str.string_match(interval_regex, v, 0)) {
                     Some(
                       Interval(
                         name,
                         Str.matched_group(1, v) |> int_of_string,
                         Str.matched_group(2, v) |> int_of_string,
                       ),
                     );
                   } else if (Str.string_match(exc_regex, v, 0)) {
                     // Upper and lower bounds are represented as binary exponents
                     let representation_to_int = l =>
                       // Avoid over/underflows for Int.min_num and Int.max_num
                       if (l >= 31) {
                         Int.max_num;
                       } else if (l <= (-31)) {
                         Int.min_num;
                       } else {
                         l |> abs |> Int.pow(2) |> Int.copysign(l);
                       };

                     Some(
                       Exclusion(
                         name,
                         Str.matched_group(2, v)
                         |> int_of_string
                         |> representation_to_int,
                         Str.matched_group(3, v)
                         |> int_of_string
                         |> representation_to_int,
                         Str.matched_group(1, v)
                         |> String.split_on_char(',')
                         |> List.remove(_, "")
                         |> List.map(int_of_string),
                       ),
                     );
                   } else {
                     int_of_string_opt(v) |> Option.map(v => Exact(name, v));
                   };
                 }
               | _ => None,
             )
        | _ => []
        };
      };

  switch (interval) {
  | `Assoc(l) => List.map(expressions_to_entries, l) |> List.concat
  | _ => []
  };
};
let value_to_name_expression_string =
  fun
  | Exact(name, value) => (name, string_of_int(value))
  | Interval(name, lower_bound, upper_bound) => (
      name,
      "["
      ++ string_of_int(lower_bound)
      ++ ","
      ++ string_of_int(upper_bound)
      ++ "]",
    )
  | Exclusion(name, lower_bound, upper_bound, exclusion_list) => (
      name,
      "["
      ++ string_of_int(lower_bound)
      ++ ","
      ++ string_of_int(upper_bound)
      ++ "] \\ "
      ++ "{"
      ++ (exclusion_list |> List.map(string_of_int) |> String.concat(","))
      ++ "}",
    );

[@react.component]
let make = (~represent: Representation.t) => {
  let var_eq = find_node(represent, "var_eq");
  let values =
    switch (var_eq) {
    | Some(`Tagged("var_eq", v)) => var_eq_to_values(v)
    | _ => []
    };

  let interval = find_node(represent, "value domain");
  let values =
    switch (interval) {
    | Some(`Tagged("value domain", v)) =>
      values @ value_domain_to_entries(v)
    | _ => values
    };
  let cmp =
    curry(
      Tuple2.mapn(value_to_name_expression_string %> fst) %> uncurry(compare),
    );

  let values =
    values
    |> List.group(cmp)
    |> List.map(l =>
         List.fold(
           // Obtain best estimate by combining all values for a variable
           ((_, lower, upper, exc), value) => {
             switch (value) {
             | Exact(n, v) => (n, v, v, [])
             | Interval(n, l, u) => (
                 n,
                 Int.max(l, lower),
                 Int.min(u, upper),
                 exc,
               )
             | Exclusion(n, l, u, e) => (
                 n,
                 Int.max(l, lower),
                 Int.min(u, upper),
                 exc @ e |> List.unique,
               )
             }
           },
           ("", min_int, max_int, []),
           l,
         )
       )
    |> List.map(((name, lower, upper, exc)) =>
         if (lower == upper) {
           Exact(name, lower);
         } else if (List.is_empty(exc)) {
           Interval(name, lower, upper);
         } else {
           Exclusion(name, lower, upper, exc);
         }
       );

  <table className="table">
    <thead>
      <tr>
        <th scope="col"> {"Name" |> React.string} </th>
        <th scope="col"> {"Expression" |> React.string} </th>
      </tr>
    </thead>
    <tbody>
      {values
       |> List.map(value_to_name_expression_string)
       |> List.map(((name, value)) => {
            <tr>
              <td> {name |> React.string} </td>
              <td> {value |> React.string} </td>
            </tr>
          })
       |> React.list}
    </tbody>
  </table>;
};
