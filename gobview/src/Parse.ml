open Util
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
type call = Call of c_id * c_file * c_line * c_order * (context * path option) list
type glob = Glob of key_value * analysis list
type warning = Warning of c_file * c_line * string
type parameters = Parameters of string
type result = Result of file list * call list * glob list * warning list
type run = Run of parameters * result

let empty_run = Run (Parameters (""), Result([],[],[],[]))

let default d = function Some x -> x | None -> d
let default_app d f = function Some x -> f x | None -> d

let rec parse_data_set c = match X.tag c with 
    | "data" -> Data (X.children c |> fun x -> List.nth_opt x 0 |> Option.map X.pcdata |> default  "" )
    | "set" -> Set (X.children c |> fun x -> List.nth_opt x 0 |> Option.map parse_key_value)
    | "map" -> Map (X.children c |> List.map parse_key_value )
    | _ -> error "Alex expected data_set"

and parse_key_value c = match X.tag c with 
    | "value" -> Value (parse_data_set @@ List.hd @@ X.children c) 
    | "key" -> Key (X.pcdata @@ List.hd @@ X.children c) 
    | _ -> error "Alex expected key or value"
let parse_analysis c = Analysis (X.attrib c "name", parse_key_value @@ List.hd @@ X.children c) 
let parse_node c = Node (X.attrib c "name")
let parse_funct c = Funct ((List.map parse_node (X.children c)),X.attrib c "name") 
let parse_file c = File (List.map parse_funct (X.children c), X.attribs c )
let parse_context c = Context (List.map parse_analysis @@ X.children c)
let parse_path c = Some (Path (List.map parse_analysis @@ X.children c))
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
        | [x], [] -> [(x, None)]
        | _ -> error "Alex expected for each context a path" in
    Call (id, file, line, order, combine (context, path))
let parse_glob c = Glob (parse_key_value @@ List.find (fun x -> X.tag x = "key") @@ X.children c, 
    List.map parse_analysis (List.filter (fun x -> X.tag x = "analysis") @@ X.children c))
let parse_parameters c = match X.tag c with 
    | "parameters" -> Parameters (List.nth (X.children c) 0 |> X.pcdata)
    | _ -> error ("Alex expected parameters "^(X.tag c))
let parse_warning c = 
    let text_tag = X.children c |> List.hd in
    let file = X.attrib text_tag "file" in
    let line = X.attrib text_tag "line" in
    let text = X.pcdata @@ List.hd @@ X.children text_tag in
    Warning (file, line, text)
let parse_result c = if X.tag c = "result" then
    let files = List.filter (fun x -> X.tag x = "file") (X.children c) in 
    let calls = List.filter (fun x -> X.tag x = "call") (X.children c) in 
    let globs = List.filter (fun x -> X.tag x = "glob") (X.children c) in 
    let warnings = List.filter (fun x -> X.tag x = "warning") (X.children c) in 
    Result (List.map parse_file files, List.map parse_call calls, List.map parse_glob globs, List.map parse_warning warnings) else error "Alex expected result"

let parse (c : X.xml) : run = match X.tag c with 
    | "run" -> let parameters = List.nth (X.children c) 0 |> parse_parameters in 
                let result = List.nth (X.children c) 1 |> parse_result in 
                Run (parameters, result)
    | _ -> error "Alex expected run tag"

let parse_string s : run = 
    let parser = XmlParser.make () in 
    XmlParser.prove parser false ;
    XmlParser.parse parser  (SString s) |> parse 

let rec list_to_kv_tuple l = if List.length l > 0 then match l with x::y::z -> [(x,y)]@(list_to_kv_tuple z) 
    | _ -> error "Alex expected for each key a value in xml map" else []
let rec data_set_to_tree = function 
    | Data s ->  T.Node(s, []) 
    | Set Some Value Data x -> T.Node (x, [])
    | Set kv -> T.Node ("set", default_app [] (fun x -> [key_value_to_tree x]) kv ) 
    | Map kvl -> T.Node("map", List.map key_value_tuple_to_tree @@ list_to_kv_tuple kvl)
and key_value_to_tree = function 
    | Value (Data(x)) -> T.Node (x, []) 
    | Value (data) -> T.Node ("value", [data_set_to_tree data])
    | Key(s) -> T.Node(s, [])
and  key_value_tuple_to_tree = function 
    | Key(s), Value Set Some Value Data x -> T.Node (s^" → "^x, [])
    | Key(s), Value (Data x) -> T.Node (s^" → "^x, []) 
    | Key(s), Value (Map kvl) -> T.Node (s, List.map key_value_tuple_to_tree @@ list_to_kv_tuple kvl)
    | Key(s), Value (data) -> T.Node (s, [data_set_to_tree data])
    | _ -> error "Alex expected proper key value pairs"

let analysis_to_tree (Analysis (name, value)) =  match value with 
    | Value (Data x) -> T.Node (name^" → "^x, [])
    | Value (Map kvl) -> T.Node (name, List.map key_value_tuple_to_tree @@ list_to_kv_tuple kvl)
    | Value (Set None) -> T.Node (name^" → ∅", [])
    | _ -> T.Node(name, [key_value_to_tree value])
let context_to_tree c = let (Context (analysis_list)) = c in T.Node("context", List.map analysis_to_tree analysis_list)
let path_to_tree (Path (analysis_list)) = T.Node("path", List.map analysis_to_tree analysis_list)

let path_to_some_tree  = function None -> [] | Some p -> [path_to_tree p]
let call_to_tree (Call (id, _, _, _, l)) = 
    let context_path_list =  if List.length l > 1 
        then List.mapi (fun i (c,p) -> T.Node("Tuple "^(string_of_int i), [context_to_tree c] @ path_to_some_tree p)) l
        else let (c,p) = List.hd l in [context_to_tree c] @ path_to_some_tree p in
    T.Node("Node:"^id ,  context_path_list)
let get_glob_name = function
        | Key (s) -> s 
        | _ -> error "Alex expected a Key not Value"
let glob_to_tree (Glob (k , analysis_list)) = T.Node(get_glob_name k, List.map analysis_to_tree analysis_list)


let glob_to_something (Glob (name , al)) something = 
    match List.find (fun (Analysis (a,_)) -> String.equal a something) al with
        | (Analysis (_,Value(Data(d)))) ->  T.Node(get_glob_name name^" → "^d, [])
        | (Analysis (_,Value (Set None))) ->  T.Node(get_glob_name name^" → ∅", [])
        | (Analysis (_,kv)) ->  T.Node(get_glob_name name, [key_value_to_tree kv])

let glob_to_inverted_tree (gl : glob list) : T.tree list = 
    let create s = T.Node(s, List.map (fun g -> glob_to_something g s) gl) in
    [create "expRelation"; create "base"; create "escape"; create "mutex"]

let file_to_name (File (_, attribs)) = List.assoc_opt "name" attribs |> default "name missing"
let file_to_path (File (_, attribs)) = List.assoc_opt "path" attribs |> default "path missing" 
let file_is_empty (File (func_list, _)) = 
    let sum = List.map (fun (Funct(nl, _)) -> List.length nl ) func_list |> List.fold_left (+) 0 in sum = 0 

let has_dead_code (Call(_,_,_,_,l)) = List.exists (fun (_, x) -> match x with None -> true | _ -> false) l
let get_line (Call(_,_,line,_,_)) = line
let get_file (Call(_,file,_,_,_)) = file
let get_calls (Run(_, Result(_,calls,_,_))) = calls
let get_globs (Run(_, Result(_,_,globs,_))) = globs
let get_files (Run(_, Result(files,_,_,_))) = files
let get_warnings (Run(_, Result(_,_,_,warnings))) = warnings 

let warning_to_file (Warning (file,_,_)) = file
let warning_to_line (Warning (_,line,_)) = line
let warning_to_text (Warning (_,_,text)) = text

let search_main_file fl = 
    let contains_main (File(l,_)) = List.exists (fun (Funct(_,name)) -> String.equal name "main") l in
    let f = List.find_opt (fun f -> contains_main f) fl in 
    (default_app "" file_to_name f, default_app "" file_to_path f)

let glob_to_inverted_tree_2 gl = 
    let add_element (l:('a * 'b) list) (glob_name, glob_value) = 
        let found,li = List.fold_left (fun (found,inner_list) (key, value) -> 
            if String.equal key glob_name then true,(key,glob_value::value)::inner_list 
            else found,(key,value)::inner_list) (false,[]) l 
        in if found then li else (glob_name, [glob_value])::li in
    let add_single_analysis (Analysis(aname, kv)) gname l =
        let x = 
            match kv with 
                | Value(Data(d)) ->  T.Node(gname^" → "^d, [])
                | Value (Set None) ->  T.Node(gname^" → ∅", [])
                | kv ->  T.Node(gname, [key_value_to_tree kv])
        in add_element l (aname, x) in
    let add_glob_to_map l (Glob (gname , al)) = let gname = get_glob_name gname in 
        List.fold_left (fun s a -> add_single_analysis a gname s ) l al in
    let gmap = List.fold_left add_glob_to_map [] gl in 
        List.map (fun (aname, l) -> T.Node(aname, l)) gmap


module ZarithTest = struct
    (* let int64num = Int64.of_int 14
    let h = IntDomain.Interval32.hash int64num *)
    (* let cilversion = Cil.cilVersion *)
    
    (* let zarith_string = 
        let zarith_test = Z.zero in 
        Z.to_string zarith_test *)
end 