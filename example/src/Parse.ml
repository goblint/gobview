module X = Xml
module T = Tree

type attribs = (string * string) list
type data_set = Data of string | Set of key_value option | Map of key_value list
and key_value = Key of string | Value of data_set
type analysis = Analysis of string * key_value 
type path = Path of analysis list
type context = Context of analysis list
type node = Node of string
type funct = Funct of node list * string
type file = File of funct list * attribs
type c_id = string
type c_file = string
type c_line = string
type c_order = string
type call = Call of c_id * c_file * c_line * c_order * (context * path) list
type glob = Glob of key_value * analysis list
type parameters = Parameters of string
type result = Result of file list * call list * glob list
type run = Run of parameters * result

let x = let x = XmlParser.make () in 
        let _ = XmlParser.prove x false in x

(* [%blob "static/data.xml"] *)
(* let xml_data = XmlParser.parse x (SString Data01.xml_data)  *)
(* let xml_data = Xml.parse_file "file:///home/alex/git/bachelor/jsoo-react-6/jsoo-react/example/src/data.xml" *)
(* let xml_data = Xml.parse_file "/data01.xml" *)

let empty_run = Run (Parameters (""), Result([],[],[]))

let default d = function Some x -> x | None -> d
let default_app d f = function Some x -> f x | None -> d

let rec parse_data_set c = match X.tag c with 
    | "data" -> Data (X.children c |> fun x -> List.nth_opt x 0 |> Option.map X.pcdata |> default  "" )
    | "set" -> Set (X.children c |> fun x -> List.nth_opt x 0 |> Option.map parse_key_value)
    | "map" -> Map (X.children c |> List.map parse_key_value )
    | _ -> failwith "Alex expected data_set"

and parse_key_value c = match X.tag c with 
    | "value" -> Value (parse_data_set @@ List.hd @@ X.children c) 
    | "key" -> Key (X.pcdata @@ List.hd @@ X.children c) 
    | _ -> failwith "Alex expected key or value"
let parse_analysis c = Analysis (X.attrib c "name", parse_key_value @@ List.hd @@ X.children c) 
let parse_node c = Node (X.attrib c "name")
let parse_funct c = Funct ((List.map parse_node (X.children c)),X.attrib c "name") 
let parse_file c = File (List.map parse_funct (X.children c), X.attribs c )
let parse_context c = Context (List.map parse_analysis @@ X.children c)
let parse_path c = Path (List.map parse_analysis @@ X.children c)
let parse_call c = 
    let id = X.attrib c "id" in
    let file = X.attrib c "file" in
    let line = X.attrib c "line" in
    let order = X.attrib c "order" in
    let context = List.map parse_context @@ List.find_all (fun x -> X.tag x = "context") @@ X.children c in
    let path = List.map parse_path @@ List.find_all (fun x -> X.tag x = "path") @@ X.children c in 
    let rec combine = function 
        | [], [] -> []
        | x::xl, y::yl -> [(x,y)] @ (combine (xl, yl) )
        | _ -> failwith "Alex expected for each context a path" in
    Call (id, file, line, order, combine (context, path))
let parse_glob c = Glob (parse_key_value @@ List.find (fun x -> X.tag x = "key") @@ X.children c, 
    List.map parse_analysis (List.filter (fun x -> X.tag x = "analysis") @@ X.children c))
let parse_parameters c = match X.tag c with 
    | "parameters" -> Parameters (List.nth (X.children c) 0 |> X.pcdata)
    | _ -> failwith ("Alex expected parameters "^(X.tag c))
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

let parse_string s : run = XmlParser.parse x (SString s) |> parse 

let rec list_to_kv_tuple l = if List.length l > 0 then match l with x::y::z -> [(x,y)]@(list_to_kv_tuple z) 
    | _ -> failwith "Alex expected for each key a value in xml map" else []
let rec data_set_to_tree = function 
    | Data s ->  T.Node(s, []) 
    | Set kv -> T.Node ("set", default_app [] (fun x -> [key_value_to_tree x]) kv ) 
    | Map kvl -> T.Node("map", List.map key_value_tuple_to_tree @@ list_to_kv_tuple kvl)
and key_value_to_tree = function 
    | Value (Data(x)) -> T.Node (x, []) 
    | Value (data) -> T.Node ("value", [data_set_to_tree data])
    | Key(s) -> T.Node(s, [])
and  key_value_tuple_to_tree = function 
    | Key(s), Value (Data x) -> T.Node (s^" → "^x, []) 
    | Key(s), Value (Map kvl) -> T.Node (s, List.map key_value_tuple_to_tree @@ list_to_kv_tuple kvl)
    | Key(s), Value (data) -> T.Node (s, [data_set_to_tree data])
    | _ -> failwith "Alex expected proper key value pairs"

let analysis_to_tree (Analysis (name, value)) =  match value with 
    | Value (Map kvl) -> T.Node (name, List.map key_value_tuple_to_tree @@ list_to_kv_tuple kvl)
    | Value (Set None) -> T.Node (name^"→ ∅", [])
    | _ -> T.Node(name, [key_value_to_tree value])
let context_to_tree c = let (Context (analysis_list)) = c in T.Node("context", List.map analysis_to_tree analysis_list)
let path_to_tree (Path (analysis_list)) = T.Node("path", List.map analysis_to_tree analysis_list)
let call_to_tree (Call (id, _, _, _, l)) = 
    let context_path_list =  if List.length l > 1 
        then List.mapi (fun i (c,p) -> T.Node("Tuple "^(string_of_int i), [context_to_tree c; path_to_tree p])) l
        else let (c,p) = List.hd l in [context_to_tree c; path_to_tree p] in
    T.Node("Node:"^id ,  context_path_list)
let get_glob_name = function
        | Key (s) -> s 
        | _ -> failwith "Alex expected a Key not Value"
let glob_to_tree (Glob (k , analysis_list)) = T.Node(get_glob_name k, List.map analysis_to_tree analysis_list)


let glob_to_something (Glob (name , al)) something = 
    match List.find (fun (Analysis (a,_)) -> String.equal a something) al with
        | (Analysis (_,Value(Data(d)))) ->  T.Node(get_glob_name name^" → "^d, [])
        | (Analysis (_,Value (Set None))) ->  T.Node(get_glob_name name^"→ ∅", [])
        | (Analysis (_,kv)) ->  T.Node(get_glob_name name, [key_value_to_tree kv])

let glob_to_inverted_tree (gl : glob list) : T.tree list = 
    let create s = T.Node(s, List.map (fun g -> glob_to_something g s) gl) in
    [create "expRelation"; create "base"; create "escape"; create "mutex"]

let file_to_name (File (_, attribs)) = List.assoc_opt "name" attribs |> default "name missing"
let file_to_path (File (_, attribs)) = List.assoc_opt "path" attribs |> default "path missing" 
let file_is_empty (File (func_list, _)) = 
    let sum = List.map (fun (Funct(nl, _)) -> List.length nl ) func_list |> List.fold_left (+) 0 in sum = 0 

let get_line (Call(_,_,line,_,_)) = line
let get_calls (Run(_, Result(_, calls, _))) = calls
let get_globs (Run(_, Result(_, _, globs))) = globs
let get_files (Run(_, Result(files, _,_))) = files

module Test = struct
    (* let int64num = Int64.of_int 14
    let h = IntDomain.Interval32.hash int64num *)
    (* let cilversion = Cil.cilVersion *)
    
    (* let zarith_string = 
        let zarith_test = Z.zero in 
        Z.to_string zarith_test *)
end 