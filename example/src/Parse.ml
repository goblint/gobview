let f () = print_endline "Hallo Welt"

module Z = XmlParser 
module X = Xml

type attribs = (string * string) list
type data_set = Data of string | Set of key_value option | Map of key_value list
and key_value = Key of string | Value of data_set
type analysis = Analysis of key_value 
type path = Path of analysis list
type context = Context of analysis list
type node = Node of string
type funct = Funct of node list * string
type file = File of funct list * attribs
type call = Call of context * path
type glob = Glob of key_value * analysis list
type parameters = Parameters of string
type result = Result of file list * call list * glob list
type run = Run of parameters * result

let extract p = (match p with Run (z,_) -> (match z with Parameters (s) -> s))

let x = Z.make () 
let y = Z.prove x false

let xml_data = Z.parse x (SString Data.xml_data)
(* let xml_data = Xml.parse_file "file:///home/alex/git/bachelor/jsoo-react-6/jsoo-react/example/src/data.xml" *)
let x = Xml.to_string_fmt xml_data
    
let b = Xml.tag xml_data 

let tag () = print_endline (x)
let rec iter a = Xml.iter (fun x -> print_endline (Xml.tag x ^ "    -    ") ; try iter x with Xml.Not_element x -> (print_endline ("NOT FOUND " ^ Xml.to_string_fmt x) )) a
let los () = iter xml_data
let default d = function Some x -> x | None -> d

let parse_key c = Key (X.pcdata @@ List.hd @@ X.children c)
let rec parse_data_set c = match X.tag c with 
    | "data" -> Data (X.children c |> fun x -> List.nth_opt x 0 |> Option.map X.pcdata |> default  "" )
    | "set" -> Set (X.children c |> fun x -> List.nth_opt x 0 |> Option.map parse_value)
    | "map" -> Map []
    | _ -> failwith "Alex expected data_set"

and parse_value c = Value (parse_data_set @@ List.hd @@ X.children c)
let parse_analysis c = Analysis (parse_value @@ List.hd @@ X.children c) 
let parse_node c = Node (X.attrib c "name")
let parse_funct c = Funct ((List.map parse_node (X.children c)),X.attrib c "name") 
let parse_file c = File (List.map parse_funct (X.children c), X.attribs c )
let parse_context c = Context (List.map parse_analysis @@ X.children c)
let parse_path c = Path (List.map parse_analysis @@ X.children c)
let parse_call c = Call (parse_context @@ List.find (fun x -> X.tag x = "context") @@ X.children c , parse_path @@ List.find (fun x -> X.tag x = "path") @@ X.children c)
let parse_glob c = Glob (parse_key @@ List.find (fun x -> X.tag x = "key") @@ X.children c, 
    List.map parse_analysis (List.filter (fun x -> X.tag x = "analysis") @@ X.children c))
let parse_parameters c = if X.tag c = "parameters" then Parameters (List.nth (X.children c) 0 |> X.pcdata  ) else failwith ("Alex expected parameters "^(X.tag c) )
let parse_result c = if X.tag c = "result" then
 let files = List.filter (fun x -> X.tag x = "file") (X.children c) in 
 let calls = List.filter (fun x -> X.tag x = "call") (X.children c) in 
 let globs = List.filter (fun x -> X.tag x = "glob") (X.children c) in 
 Result (List.map parse_file files, List.map parse_call calls, List.map parse_glob globs) else failwith "Alex expected result"

let parse (c : X.xml) : run = match X.tag c with 
    | "run" -> let parameters = List.nth (X.children c) 0 |> parse_parameters in 
                let result = List.nth (X.children c) 1 |> parse_result in 
                Run (parameters, result)
                
    | _ -> failwith "Alex expected run tag"


(* let _ = parse xml_data *)