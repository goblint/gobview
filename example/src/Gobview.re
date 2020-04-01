type action =
  | Increment
  | Decrement;

let z = Parse.parse(Parse.xml_data);

let reducer = (state, action) =>
  switch (action) {
  | Increment => state + 1
  | Decrement => state - 1
  };

let space = {
  " " |> React.string;
};
let treez = Parse.parse (Parse.xml_data )|> Parse.get_calls |> List.hd |> Parse.call_to_tree;

[@react.component]
let make = () => {
  <div>
    <ul><TreeView tree=treez /></ul>
  </div>;
};
