"(mac = args (cons 'assign args))\n"
"\n"
"(= list (fn args args))\n"
"\n"
"(mac def (name args . body) (list '= name (cons 'fn (cons args body))))\n"
"\n"
"(def rreduce (f xs)\n"
"\"Like [[reduce]] but accumulates elements of 'xs' in reverse order.\"\n"
"  (if (cddr xs)\n"
"    (f (car xs) (rreduce f (cdr xs)))\n"
"    (apply f xs)))\n"
"\n"
"(def no (x) (is x nil))\n"
"\n"
"(def complement (f)\n"
"\"Returns a function that behaves as if the result of calling 'f' was negated.\n"
"For example, this is always true:\n"
"  ((complement f) a b) <=> (no (f a b))\"\n"
"  (fn args (no (apply f args))))\n"
"\n"
"(def isa (x y)\n"
"	(is (type x) y))\n"
"\n"
"(def isnt (x y) (no (is x y)))\n"
"\n"
"(def abs (x) (if (< x 0) (- 0 x) x))\n"
"\n"
"(def reduce (f xs)\n"
"\"Accumulates elements of 'xs' using binary function 'f'.\"\n"
"  (if (cddr xs)\n"
"    (reduce f (cons (f car.xs cadr.xs)\n"
"                    cddr.xs))\n"
"    (apply f xs)))\n"
"\n"
"(def map1 (f xs)\n"
"\"Returns a list containing the result of function 'f' applied to every element of 'xs'.\"\n"
"  (if (no xs)\n"
"    nil\n"
"    (cons (f (car xs))\n"
"          (map1 f (cdr xs)))))\n"
"\n"
"(def caar (x) (car (car x)))\n"
"(def cadr (x) (car (cdr x)))\n"
"(def cddr (x) (cdr (cdr x)))\n"
"\n"
"(mac and2 (a b) (list 'if a b nil))\n"
"(mac or (a b) (list 'if a t b))\n"
"\n"
"(mac quasiquote (x)\n"
"  (if (isa x 'cons)\n"
"      (if (is (car x) 'unquote)\n"
"          (cadr x)\n"
"          (if (and2 (isa (car x) 'cons) (is (caar x) 'unquote-splicing))\n"
"              (list '+\n"
"                    (cadr (car x))\n"
"                    (list 'quasiquote (cdr x)))\n"
"              (list 'cons\n"
"                    (list 'quasiquote (car x))\n"
"                    (list 'quasiquote (cdr x)))))\n"
"      (list 'quote x)))\n"
"\n"
"(mac let (sym def . body)\n"
"	`((fn (,sym) ,@body) ,def))\n"
"\n"
"(mac do body\n"
"	`((fn () ,@body)))\n"
"\n"
"(mac rfn (name parms . body)\n"
"\"Like [[fn]] but permits the created function to call itself recursively as the given 'name'.\"\n"
"  `(let ,name nil\n"
"     (assign ,name (fn ,parms ,@body))))\n"
"\n"
"(def rev (xs)\n"
"\"Returns a list containing the elements of 'xs' back to front.\"\n"
"  ((rfn recur (xs acc)\n"
"    (if (no xs)\n"
"      acc\n"
"      (recur cdr.xs\n"
"             (cons car.xs acc)))) xs nil))\n"
"\n"
"(def pair (xs (o f list))\n"
"  \"Splits the elements of 'xs' into buckets of two, and optionally applies the\n"
"function 'f' to them.\"\n"
"  (if (no xs)\n"
"       nil\n"
"      (no cdr.xs)\n"
"       (list (list car.xs))\n"
"      (cons (f car.xs cadr.xs)\n"
"            (pair cddr.xs f))))\n"
"\n"
"(mac with (parms . body)\n"
"  `((fn ,(map1 car (pair parms))\n"
"     ,@body)\n"
"    ,@(map1 cadr (pair parms))))\n"
"\n"
"(def join args\n"
"  (if (no args)\n"
"    nil\n"
"    (let a (car args)\n"
"      (if (no a)\n"
"        (apply join (cdr args))\n"
"        (cons (car a) (apply join (cons (cdr a) (cdr args))))))))\n"
"\n"
"(= uniq (let uniq-count 0\n"
"  (fn () (sym (string \"_uniq\" (= uniq-count (+ uniq-count 1)))))))\n"
"\n"
"(mac w/uniq (names . body)\n"
"  (if (isa names 'cons)\n"
"    `(with ,(apply join (map1 (fn (x) (list x '(uniq))) names))\n"
"       ,@body)\n"
"    `(let ,names (uniq) ,@body)))\n"
"\n"
"(mac when (test . body)\n"
"	 (list 'if test (cons 'do body)))\n"
"\n"
"(mac while (test . body)\n"
"  \"Executes body repeatedly while test is true. The test is evaluated before each execution of body.\"\n"
"  (let f (uniq)\n"
"    `(let ,f nil\n"
"    (assign ,f (fn ()\n"
"      (when ,test\n"
"        ,@body\n"
"        (,f))))\n"
"    (,f))))\n"
"\n"
"(mac each (var expr . body)\n"
"     (w/uniq (seq i)\n"
"	     `(let ,seq ,expr\n"
"		   (if (isa ,seq 'cons) (while ,seq (= ,var (car ,seq)) ,@body (= ,seq (cdr ,seq)))\n"
"		       (isa ,seq 'table) (maptable (fn ,var ,@body) ,seq)\n"
"		       'else (let ,i 0 (while (isnt (,seq ,i) #\\nul) (= ,var (,seq ,i)) ,@body (++ ,i)))))))\n"
"\n"
"(mac and args\n"
"\"Stops at the first argument to fail (return nil). Returns the last argument before stopping.\"\n"
"  (if args\n"
"    (if (cdr args)\n"
"      `(if ,(car args) (and ,@(cdr args)))\n"
"      (car args))\n"
"    t))\n"
"\n"
"(mac or args\n"
"\"Stops at the first argument to pass, and returns its result.\"\n"
"  (and args\n"
"       (w/uniq g\n"
"         `(let ,g ,(car args)\n"
"            (if ,g ,g\n"
"              (or ,@(cdr args)))))))\n"
"\n"
"(def iso (x y)\n"
"\"Are 'x' and 'y' equal-looking to each other? Non-atoms like lists and tables can contain\n"
"the same elements (be *isomorphic*) without being identical.\"\n"
"  (or (is x y)\n"
"      (and (acons x)\n"
"           (acons y)\n"
"           (iso (car x) (car y))\n"
"           (iso (cdr x) (cdr y)))))\n"
"\n"
"(def <= args\n"
"\"Is each element of 'args' lesser than or equal to all following elements?\"\n"
"  (or (no args)\n"
"      (no (cdr args))\n"
"      (and (no (> (car args) (cadr args)))\n"
"           (apply <= (cdr args)))))\n"
"\n"
"(def >= args\n"
"\"Is each element of 'args' greater than or equal to all following elements?\"\n"
"  (or (no args)\n"
"      (no (cdr args))\n"
"      (and (no (< (car args) (cadr args)))\n"
"           (apply >= (cdr args)))))\n"
"\n"
"(mac ++ (place (o i 1))\n"
"  (if (isa place 'cons)\n"
"    (w/uniq (a head index default)\n"
"      (if (is (car place) 'car) `(let ,a ,(cadr place) (scar ,a (+ (car ,a) ,i)))\n"
"	  (if (is (car place) 'cdr) `(let ,a ,(cadr place) (scdr ,a (+ (cdr ,a) ,i)))\n"
"	      (if (cddr place)\n"
"		  `(with (,head ,(car place)\n"
"				,index ,(cadr place)\n"
"				,default ,(cadr (cdr place)))\n"
"		     (sref ,head (+ (,head ,index ,default) ,i) ,index))\n"
"		  'else\n"
"		  `(with (,head ,(car place)\n"
"				,index ,(cadr place))\n"
"		     (sref ,head (+ (,head ,index) ,i) ,index)))\n"
"	  )))\n"
"    `(assign ,place (+ ,place ,i))))\n"
"\n"
"(mac -- (place (o i 1))\n"
"  (if (isa place 'cons)\n"
"    (w/uniq (a head index default)\n"
"      (if (is (car place) 'car) `(let ,a ,(cadr place) (scar ,a (- (car ,a) ,i)))\n"
"	  (if (is (car place) 'cdr) `(let ,a ,(cadr place) (scdr ,a (- (cdr ,a) ,i)))\n"
"	      (if (cddr place)\n"
"		  `(with (,head ,(car place)\n"
"				,index ,(cadr place)\n"
"				,default ,(cadr (cdr place)))\n"
"		     (sref ,head (- (,head ,index ,default) ,i) ,index))\n"
"		  'else\n"
"		  `(with (,head ,(car place)\n"
"				,index ,(cadr place))\n"
"		     (sref ,head (- (,head ,index) ,i) ,index)))\n"
"	  )))\n"
"    `(assign ,place (- ,place ,i))))\n"
"\n"
"(def nthcdr (n pair)\n"
"	(let i 0\n"
"		(while (and (< i n) pair)\n"
"			(= pair (cdr pair))\n"
"			(++ i)))\n"
"	pair)\n"
"\n"
"; = place value ...\n"
"(mac = args\n"
"     (cons 'do (map1 (fn (p) (with (place (car p) value (cadr p))\n"
"			      (if (isa place 'cons)\n"
"				  (if (is (car place) 'car)\n"
"				      (list 'scar (cadr place) value)\n"
"				      (if (is (car place) 'cdr)\n"
"					  (list 'scdr (cadr place) value)\n"
"					  (list 'sref (car place) value (cadr place))))\n"
"				  (list 'assign place value))))\n"
"		    (pair args))))\n"
"\n"
"(mac unless (test . body)\n"
"  `(if (no ,test) (do ,@body)))\n"
"\n"
"(mac do1 xs `(let it ,(car xs) ,@(cdr xs) it))\n"
"\n"
"(def pr args\n"
"	\"Prints all its 'args' to screen. Returns the first arg.\"\n"
"  (map1 disp args)\n"
"  (car args))\n"
"\n"
"(def prn xs (do1 (apply pr xs) (writeb 10)))\n"
"\n"
"(mac for (var init max . body)\n"
"  (w/uniq g\n"
"  `(let ,g ,max (= ,var ,init)\n"
"      (while (<= ,var ,g) ,@body (++ ,var)))))\n"
"\n"
"(def idfn (x) x)\n"
"\n"
"(def number (n)\n"
"  \"Is 'n' a number?\"\n"
"  (is (type n) 'num))\n"
"\n"
"(def positive (x)\n"
"  (and (number x) (> x 0)))\n"
"\n"
"(mac withs (parms . body)\n"
"	 \"Like [[with]], but binding for a variable can refer to earlier variables.\n"
"For example, (withs (x 1 y (+ x 1))\n"
"               (+ x y))\n"
"             => 3\"\n"
"	 (if (no parms)\n"
"		 `(do ,@body)\n"
"		 `(let ,(car parms) ,(cadr parms)\n"
"			   (withs ,(cddr parms) ,@body))))\n"
"\n"
"(def even (n)\n"
"\"Is n even?\"\n"
"  (is (mod n 2) 0))\n"
"\n"
"(def odd (n)\n"
"\"Is n odd?\"\n"
"  (no (even n)))\n"
"\n"
"(def round (n)\n"
"\"Approximates a fractional value to the nearest even integer.\n"
"Negative numbers are always treated exactly like their positive variants\n"
"barring the sign.\"\n"
"  (withs (base (trunc n) rem (abs (- n base)))\n"
"    (if (> rem 0.5) ((if (> n 0) + -) base 1)\n"
"        (< rem 0.5) base\n"
"        (odd base)  ((if (> n 0) + -) base 1)\n"
"		base)))\n"
"\n"
"(def roundup (n)\n"
"\"Like [[round]] but halves are rounded up rather than down.\"\n"
"  (withs (base (trunc n) rem (abs (- n base)))\n"
"    (if (>= rem 0.5)\n"
"      ((if (> n 0) + -) base 1)\n"
"      base)))\n"
"\n"
"(def nearest (n quantum)\n"
"  \"Like [[round]] but generalized to arbitrary units.\"\n"
"  (* (roundup (/ n quantum)) quantum))\n"
"\n"
"(def avg (ns)\n"
"  \"Returns the arithmetic mean of a list of numbers 'ns'.\"\n"
"  (/ (apply + ns) (len ns)))\n"
"\n"
"(def multiple (x y)\n"
"  \"Is 'x' a multiple of 'y'?\"\n"
"  (is 0 (mod x y)))\n"
"\n"
"(def carif (x)\n"
"  \"Returns the first element of the given list 'x', or just 'x' if it isn't a list.\"\n"
"  (if (is (type x) 'cons) (car x) x))\n"
"\n"
"(mac iflet (var expr . branches)\n"
"\"If 'expr' is not nil, binds 'var' to it before running the first branch.\n"
"Can be given multiple alternating test expressions and branches. The first\n"
"passing test expression is bound to 'var' before running its corresponding branch.\n"
"\n"
"For examples, see [[aif]].\"\n"
"  (if branches\n"
"    (w/uniq gv\n"
"      `(let ,gv ,expr\n"
"         (if ,gv\n"
"           (let ,var ,gv\n"
"             ,(car branches))\n"
"           ,(if (cdr branches)\n"
"              `(iflet ,var ,@(cdr branches))))))\n"
"    expr))\n"
"\n"
"(mac whenlet (var expr . body)\n"
"	 \"Like [[when]] but also puts the value of 'expr' in 'var' so 'body' can access it.\"\n"
"	 `(iflet ,var ,expr (do ,@body)))\n"
"\n"
"(def best (f seq)\n"
"  \"Maximizes comparator function 'f' throughout seq.\"\n"
"  (whenlet wins (carif seq)\n"
"		   (each elt (cdr seq)\n"
"				 (if (f elt wins)\n"
"					 (= wins elt)))\n"
"		   wins))\n"
"\n"
"(def max args\n"
"  \"Returns the greatest of 'args'.\"\n"
"  (best > args))\n"
"\n"
"(def min args\n"
"  \"Returns the least of 'args'.\"\n"
"  (best < args))\n"
"\n"
"(def firstn (n xs)\n"
"	\"Returns the first 'n' elements of 'xs'.\"\n"
"  (if (no n)            xs\n"
"      (and (> n 0) xs)  (cons (car xs) (firstn (- n 1) (cdr xs)))\n"
"			nil))\n"
"\n"
"(mac afn (parms . body)\n"
"\"Like [[fn]] and [[rfn]] but the created function can call itself as 'self'\"\n"
"  `(rfn self ,parms ,@body))\n"
"\n"
"(mac compose args\n"
"\"Takes a list of functions and returns a function that behaves as if all its\n"
"'args' were called in sequence.\n"
"For example, this is always true:\n"
"  ((compose f g h) a b c) <=> (f (g (h a b c))).\n"
"Be wary of passing macros to compose.\"\n"
"  (w/uniq g\n"
"    `(fn ,g\n"
"       ,((afn (fs)\n"
"          (if cdr.fs\n"
"            (list car.fs (self cdr.fs))\n"
"            `(apply ,(if car.fs car.fs 'idfn) ,g))) args))))\n"
"\n"
"; Destructive stable merge-sort, adapted from slib and improved\n"
"; by Eli Barzilay for MzLib; re-written in Arc.\n"
"\n"
"(def mergesort (less? lst)\n"
"  (with (n (len lst))\n"
"    (if (<= n 1) lst\n"
"        ((rfn recur (n)\n"
"           (if (> n 2)\n"
"                ; needs to evaluate L->R\n"
"                (withs (j (/ (if (even n) n (- n 1)) 2) ; faster than round\n"
"                        a (recur j)\n"
"                        b (recur (- n j)))\n"
"                  (merge less? a b))\n"
"               ; the following case just inlines the length 2 case,\n"
"               ; it can be removed (and use the above case for n>1)\n"
"               ; and the code still works, except a little slower\n"
"               (is n 2)\n"
"                (with (x (car lst) y (cadr lst) p lst)\n"
"                  (= lst (cddr lst))\n"
"                  (when (less? y x) (scar p y) (scar (cdr p) x))\n"
"                  (scdr (cdr p) nil)\n"
"                  p)\n"
"               (is n 1)\n"
"                (with (p lst)\n"
"                  (= lst (cdr lst))\n"
"                  (scdr p nil)\n"
"                  p)\n"
"               nil)) n))))\n"
"\n"
"; Also by Eli.\n"
"\n"
"(def merge (less? x y)\n"
"  (if (no x) y\n"
"      (no y) x\n"
"      (let lup nil\n"
"        (assign lup\n"
"                (fn (r x y r-x?) ; r-x? for optimization -- is r connected to x?\n"
"                  (if (less? (car y) (car x))\n"
"                    (do (if r-x? (scdr r y))\n"
"                        (if (cdr y) (lup y x (cdr y) nil) (scdr y x)))\n"
"                    ; (car x) <= (car y)\n"
"                    (do (if (no r-x?) (scdr r x))\n"
"                        (if (cdr x) (lup x (cdr x) y t) (scdr x y))))))\n"
"        (if (less? (car y) (car x))\n"
"          (do (if (cdr y) (lup y x (cdr y) nil) (scdr y x))\n"
"              y)\n"
"          ; (car x) <= (car y)\n"
"          (do (if (cdr x) (lup x (cdr x) y t) (scdr x y))\n"
"              x)))))\n"
"\n"
"(def acons (x)\n"
"\"Is 'x' a non-nil list?\"\n"
"  (is (type x) 'cons))\n"
"\n"
"(def alist (x)\n"
"\"Is 'x' a (possibly empty) list?\"\n"
"(or (no x) (acons x)))\n"
"\n"
"(mac in (x . choices)\n"
"\"Does 'x' match one of the given 'choices'?\"\n"
"  (w/uniq g\n"
"    `(let ,g ,x\n"
"       (or ,@(map1 (fn (c) `(is ,g ,c))\n"
"                   choices)))))\n"
"\n"
"(def atom (x)\n"
"\"Is 'x' a simple type? (i.e. not list, table or user-defined)\"\n"
"  (in (type x) 'int 'num 'sym 'char 'string))\n"
"\n"
"(def copy (x)\n"
"\"Creates a deep copy of 'x'. Future changes to any part of 'x' are guaranteed\n"
"to be isolated from the copy.\"\n"
"  (if (atom x)\n"
"    x\n"
"    (cons (copy (car x))\n"
"          (copy (cdr x)))))\n"
"\n"
"; Use mergesort on assumption that mostly sorting mostly sorted lists\n"
"(def sort (test seq)\n"
"\"Orders a list 'seq' by comparing its elements using 'test'.\"\n"
"  (if (alist seq)\n"
"    (mergesort test (copy seq))\n"
"    (coerce (mergesort test (coerce seq 'cons)) (type seq))))\n"
"\n"
"(def med (ns (o test >))\n"
"	\"Returns the median of a list of numbers 'ns' according to the comparison 'test'. Takes the later element for an even-length list.\"\n"
"	((sort test ns) (trunc (/ (len ns) 2))))\n"
"\n"
"(def median (ns)\n"
"	\"Returns the median of the list (the element at the midpoint of the list when sorted highest-to-lowest). Takes the earlier element for an even-length list.\"\n"
"	((sort < ns) (trunc (/ (len ns) 2))))\n"
"\n"
"(def testify (x)\n"
"\"Turns an arbitrary value 'x' into a predicate function to compare with 'x'.\"\n"
"  (if (isa x 'fn) x [iso _ x]))\n"
"\n"
"(def reclist (f xs)\n"
"\"Calls function 'f' with successive [[cdr]]s of 'xs' until one of the calls passes.\"\n"
"  (and xs (or (f xs) (if (acons xs) (reclist f (cdr xs))))))\n"
"\n"
"(mac check (x test (o alt))\n"
"\"Returns `x' if it satisfies `test', otherwise returns 'alt' (nil if it's not provided).\"\n"
"  (w/uniq gx\n"
"    `(let ,gx ,x\n"
"       (if (,test ,gx) ,gx ,alt))))\n"
"\n"
"(def find (test seq)\n"
"\"Returns the first element of 'seq' that satisfies `test'.\"\n"
"  (let f (testify test)\n"
"    (reclist [check (carif _) f] seq)))\n"
"\n"
"(def get (i)\n"
"	\"Returns a function to pass 'i' to its input.\n"
"Useful in higher-order functions, or to index into lists, strings, tables, etc.\"\n"
"	[_ i])\n"
"\n"
"; Syntax expansion is done by reader.\n"
"(def ssexpand (symbol) symbol)\n"
"\n"
"(def fill-table (table data)\n"
"\"Populates 'table' with alternating keys and values in 'data'.\"\n"
"  (do1 table\n"
"       (each p pair.data\n"
"	     (with (k (car p) v (cadr p))\n"
"	       (= table.k v)))))\n"
"\n"
"(def keys (h)\n"
"  \"Returns list of keys in table 'h'.\"\n"
"  (let r nil\n"
"    (maptable (fn (k v) (= r (cons k r))) h) r))\n"
"\n"
"(def vals (h)\n"
"  \"Returns list of values in table 'h'.\"\n"
"  (let r nil\n"
"    (maptable (fn (k v) (= r (cons v r))) h) r))\n"
"\n"
"(def tablist (h)\n"
"  \"Converts table 'h' into an association list of (key value) pairs. Reverse of [[listtab]].\"\n"
"  (let r nil\n"
"    (maptable (fn p (= r (cons p r))) h) r))\n"
"\n"
"(def listtab (al)\n"
"  \"Converts association list 'al' of (key value) pairs into a table. Reverse of [[tablist]].\"\n"
"  (let h (table)\n"
"    (map1 (fn (p) (with (k (car p) v (cadr p)) (= (h k) v)))\n"
"         al)\n"
"    h))\n"
"\n"
"(mac obj args\n"
"\"Creates a table out of a list of alternating keys and values.\"\n"
"  `(listtab (list ,@(map1 (fn (p) (with (k (car p) v (cadr p))\n"
"				   `(list ',k ,v)))\n"
"                         (pair args)))))\n"
"\n"
"(mac caselet (var expr . args)\n"
"\"Like [[case]], but 'expr' is also bound to 'var' and available inside the 'args'.\"\n"
"  `(let ,var ,expr\n"
"     ,((afn (args)\n"
"        (if (no cdr.args)\n"
"          car.args\n"
"          `(if (is ,var ',car.args)\n"
"             ,cadr.args\n"
"             ,(self cddr.args)))) args)))\n"
"\n"
"(mac case (expr . args)\n"
"\"Usage: (case expr test1 then1 test2 then2 ...)\n"
"Matches 'expr' to the first satisfying 'test' and runs the corresponding 'then' branch.\"\n"
"  `(caselet ,(uniq) ,expr ,@args))\n"
"\n"
"(mac w/table (var . body)\n"
"     \"Runs 'body' to add to table 'var' and finally return it.\"\n"
"     `(let ,var (table) ,@body ,var))\n"
"\n"
"(def memtable ((o keys nil) (o val t))\n"
"  \"Turns a list into a table indicating membership of all elements.\"\n"
"  (w/table tbl\n"
"    (each key keys\n"
"      (= tbl.key val))))\n"
"\n"
"(def pos (test seq (o start 0))\n"
"  \"Returns the index of the first element of 'seq' matching 'test', starting\n"
"from index 'start' (0 by default).\"\n"
"  (with (f testify.test seq (coerce seq 'cons))\n"
"    ((afn (seq n)\n"
"	  (if (no seq)\n"
"	      nil\n"
"	      (f car.seq)\n"
"	      n\n"
"	      (self cdr.seq (+ n 1)))) (nthcdr start seq) start)))\n"
"\n"
"(def trues (f xs)\n"
"\"Returns (map1 f xs) dropping any nils.\"\n"
"  (and xs\n"
"       (iflet fx (f car.xs)\n"
"         (cons fx (trues f cdr.xs))\n"
"         (trues f cdr.xs))))\n"
"\n"
"(def rem (test seq)\n"
"  \"Returns all elements of 'seq' except those satisfying 'test'.\"\n"
"  (with (f (testify test) type* (type seq))\n"
"    (coerce\n"
"     ((afn (s)\n"
"	   (if (no s)        nil\n"
"	       (f car.s)     (self cdr.s)\n"
"	       'else         (cons car.s (self cdr.s)))) (coerce seq 'cons)) type*)))\n"
"\n"
"(def keep (test seq)\n"
"  \"Returns all elements of 'seq' for which 'test' passes.\"\n"
"  (rem (complement (testify test)) seq))\n"
"\n"
"(def assoc (key al)\n"
"  \"Finds a (key value) pair in an association list 'al' of such pairs.\"\n"
"  (if (no acons.al) nil\n"
"      (and (acons (car al)) (is (caar al) key)) (car al)\n"
"      (assoc key (cdr al))))\n"
"\n"
"(def alref (al key)\n"
"  \"Returns the value of 'key' in an association list 'al' of (key value) pairs\"\n"
"  (cadr (assoc key al)))\n"
"\n"
"(mac wipe args\n"
"\"Sets each place in 'args' to nil.\"\n"
"  `(do ,@(map1 (fn (a) `(= ,a nil)) args)))\n"
"\n"
"(mac set args\n"
"  \"Sets each place in 'args' to t.\"\n"
"  `(do ,@(map1 (fn (a) `(= ,a t)) args)))\n"
"\n"
"(mac aif (expr . branches)\n"
"\"Like [[if]], but also puts the value of 'expr' in variable 'it'.\"\n"
"  `(iflet it ,expr ,@branches))\n"
"\n"
"(mac swap (place1 place2)\n"
"  \"Exchanges the values of 'place1' and 'place2'.\"\n"
"  (w/uniq g\n"
"    `(let ,g ,place1 (= ,place1 ,place2) (= ,place2 ,g))))\n"
"\n"
"(mac rotate places\n"
"		 \"Like [[swap]] but for more than two places.\n"
"For example, after (rotate place1 place2 place3), place3 is moved to place2,\n"
"place2 to place1, and place1 to place3.\"\n"
"		 (if (no places) nil\n"
"				 (w/uniq g\n"
"				 (let binds* nil\n"
"					 ((afn (x) (when x (push (list = (car x) (aif (cdr x) (car it) g)) binds*) (self (cdr x)))) places)\n"
"					 `(let ,g ,(car places) ,@(rev binds*))))))\n"
"\n"
"(mac zap (op place . args)\n"
"  \"Replaces 'place' with (op place args...)\"\n"
"  `(= ,place (,op ,place ,@args)))\n"
"\n"
"(mac push (x place)\n"
"  \"Adds 'x' to the start of the sequence at 'place'.\"\n"
"  `(= ,place (cons ,x ,place)))\n"
"\n"
"(mac pop (place)\n"
"  \"Opposite of [[push]]: removes the first element of the sequence at 'place' and returns it.\"\n"
"  `(= ,place (cdr ,place)))\n"
"\n"
"(mac pull (test place)\n"
"  \"Removes all elements from 'place' that satisfy 'test'.\"\n"
"	`(= ,place (rem ,test ,place)))\n"
"\n"
"(def recstring (test s (o start 0))\n"
"\"Calls function 'test' with successive characters in string 's' until one of the calls passes.\"\n"
"  ((afn ((o i start))\n"
"    (and (< i len.s)\n"
"         (or test.i\n"
"             (self (+ i 1)))))))\n"
"\n"
"(def some (test seq)\n"
"  \"Does at least one element of 'seq' satisfy 'test'?\"\n"
"  (let f testify.test\n"
"    (if (isa seq 'string)\n"
"	(recstring f:seq seq)\n"
"	(reclist f:carif seq))))\n"
"\n"
"(def all (test seq)\n"
"  \"Does every element of 'seq' satisfy 'test'?\"\n"
"  (~some (complement (testify test)) seq))\n"
"\n"
"(def adjoin (x xs)\n"
"  (if (some x xs)\n"
"    xs\n"
"    (cons x xs)))\n"
"\n"
"(mac pushnew (x place)\n"
"  \"Like [[push]] but first checks if 'x' is already present in 'place'.\"\n"
"	`(= ,place (adjoin ,x ,place)))\n"
"\n"
"(mac nor args\n"
"  \"Computes args until one of them passes, then returns nil.\n"
"Returns t if none of the args passes.\"\n"
"  `(no (or ,@args)))\n"
"\n"
"(mac until (test . body)\n"
"\"Like [[while]], but negates 'test'; loops through 'body' as long as 'test' fails.\"\n"
"  `(while (no ,test) ,@body))\n"
"\n"
"(mac whilet (var test . body)\n"
"  \"Like [[while]], but successive values of 'test' are bound to 'var'.\"\n"
"  `(let ,var nil\n"
"	(while (= ,var ,test) ,@body)))\n"
"\n"
"(mac whiler (var expr end . body)\n"
"\"Repeatedly binds 'var' to 'expr' and runs 'body' until 'var' matches 'end'.\"\n"
"  (w/uniq gendf\n"
"    `(withs (,var nil ,gendf (testify ,end))\n"
"       (while (no (,gendf (= ,var ,expr)))\n"
"         ,@body))))\n"
"\n"
"(mac loop (start test update . body)\n"
"     \"Executes start, then executes body repeatedly, checking test before each iteration and executing update afterward.\"\n"
"     `(do ,start\n"
"	  (while ,test ,@body ,update)))\n"
"\n"
"(mac accum (accfn . body)\n"
"\"Runs 'body' (usually containing a loop) and then returns in order all the\n"
"values that were called with 'accfn' in the process.\n"
"Can be cleaner than map for complex anonymous functions.\"\n"
"  (w/uniq gacc\n"
"    `(withs (,gacc nil ,accfn [push _ ,gacc])\n"
"       ,@body\n"
"       (rev ,gacc))))\n"
"\n"
"(mac drain (expr (o eos nil))\n"
"\"Repeatedly evaluates 'expr' until it returns 'eos' (nil by default). Returns\n"
"a list of the results.\"\n"
"  (w/uniq (gacc gres)\n"
"    `(accum ,gacc\n"
"       (whiler ,gres ,expr ,eos\n"
"         (,gacc ,gres)))))\n"
"\n"
"(mac repeat (n . body)\n"
"     \"Runs 'body' expression by expression 'n' times.\"\n"
"     (w/uniq g\n"
"	     `(for ,g 1 ,n ,@body)))\n"
"\n"
"(mac forlen (var s . body)\n"
"     \"Loops through the length of sequence 's', binding each element to 'var'.\"\n"
"     `(for ,var 0 (- (len ,s) 1) ,@body))\n"
"\n"
"(mac noisy-each (n var val . body)\n"
"\"Like [[each]] but print a progress indicator every 'n' iterations.\"\n"
"  (w/uniq (gn gc)\n"
"    `(with (,gn ,n ,gc 0)\n"
"       (each ,var ,val\n"
"         (when (multiple (++ ,gc) ,gn)\n"
"           (pr \".\")\n"
"           (flushout)\n"
"           )\n"
"         ,@body)\n"
"       (prn)\n"
"       (flushout))))\n"
"\n"
"(mac on (var s . body)\n"
"\"Like [[each]], but also maintains a variable calles 'index' counting the iterations.\"\n"
"  (if (is var 'index)\n"
"    (err \"Can't use index as first arg to on.\")\n"
"    (w/uniq gs\n"
"      `(let ,gs ,s\n"
"         (forlen index ,gs\n"
"           (let ,var (,gs index)\n"
"             ,@body))))))\n"
"\n"
"(mac ontable (k v tab . body)\n"
"     \"Iterates over the table tab, assigning k and v each key and value.\"\n"
"     `(maptable (fn (,k ,v) ,@body) ,tab))\n"
"\n"
"(def empty (seq)\n"
"  \"Is 'seq' an empty container? Usually checks 'seq's [[len]].\"\n"
"  (iso 0 len.seq))\n"
"\n"
"(def orf fns\n"
"\"Returns a function which calls all the functions in 'fns' on its args, and\n"
"[[or]]s the results. ((orf f g) x y) <=> (or (f x y) (g x y))\"\n"
"  (fn args\n"
"    ((afn ((o fs fns))\n"
"      (and fs\n"
"           (or (apply car.fs args)\n"
"               (self cdr.fs)))))))\n"
"\n"
"(def andf fns\n"
"\"Returns a function which calls all the functions in 'fns' on its args, and\n"
"[[and]]s the results. For example, ((andf f g) x y) <=> (and (f x y) (g x y)).\n"
"Simple syntax: f&g <=> (andf f g)\"\n"
"  (fn args\n"
"    ((afn ((o fs fns))\n"
"      (if no.fs          t\n"
"          (no cdr.fs)    (apply car.fs args)\n"
"          'else          (and (apply car.fs args)\n"
"                              (self cdr.fs)))))))\n"
"\n"
"(def atend (i s)\n"
"\"Is index 'i' at or past the end of sequence 's'?\"\n"
"  (>= i (- len.s 1)))\n"
"\n"
"(mac aand args\n"
"\"Like [[and]], but each expression in 'args' can access the result of the\n"
"previous one in variable 'it'.\"\n"
"  (if (no args)\n"
"       t\n"
"      (no (cdr args))\n"
"       (car args)\n"
"      `(let it ,(car args) (and it (aand ,@(cdr args))))))\n"
"\n"
"(def dotted (x)\n"
"\"Is 'x' an _improper_ list terminating in something other than nil?\n"
"Name comes from (cons 1 2) being printed with a dot: (1 . 1).\"\n"
"  (aand acons.x\n"
"        cdr.x\n"
"        ((orf ~acons dotted) it)))\n"
"\n"
"(mac conswhen (f x y)\n"
"\"Adds 'x' to the front of 'y' if 'x' satisfies test 'f'.\"\n"
"  (w/uniq (gf gx)\n"
"   `(with (,gf ,f  ,gx ,x)\n"
"      (if (,gf ,gx) (cons ,gx ,y) ,y))))\n"
"\n"
"(def consif (x xs)\n"
"  \"Like [[cons]] on 'x' and 'xs' unless 'x' is nil.\"\n"
"  (if x (cons x xs) xs))\n"
"\n"
"(def last (xs)\n"
"  \"Returns the last element of 'xs'.\"\n"
"  (if (cdr xs)\n"
"    (last (cdr xs))\n"
"    (car xs)))\n"
"\n"
"(def flat x\n"
"  \"Flattens a list of lists.\"\n"
"  ((afn ((o x x) (o acc nil))\n"
"    (if no.x        acc\n"
"        (~acons x)  (cons x acc)\n"
"        'else       (self car.x (self cdr.x acc))))))\n"
"\n"
"(def caris (x val)\n"
"  (and (acons x) (is (car x) val)))\n"
"\n"
"; common uses of map\n"
"(def mappend (f . args)\n"
"\"Like [[map]] followed by append.\"\n"
"  (apply + (apply + (map1 [map f _] args))))\n"
"\n"
"(def range-bounce (i max)\n"
"\"Munges index 'i' in slices of a sequence of length 'max'. First element starts\n"
" at index 0. Negative indices count from the end. A nil index denotes the end.\"\n"
"  (if (no i)  max\n"
"      (< i 0)  (+ max i)\n"
"      (>= i max) max\n"
"      'else  i))\n"
"\n"
"(def cut (seq start (o end))\n"
"\"Extract a chunk of 'seq' from index 'start' (inclusive) to 'end' (exclusive). 'end'\n"
"can be left out or nil to indicate everything from 'start', and can be\n"
"negative to count backwards from the end.\"\n"
"  (firstn (- (range-bounce end len.seq)\n"
"             start)\n"
"          (nthcdr start seq)))\n"
"\n"
"(def split (seq pos)\n"
"  \"Partitions 'seq' at index 'pos'.\"\n"
"	(list (cut seq 0 pos) (cut seq pos)))\n"
"\n"
"; Generalization of pair: (tuples x) = (pair x)\n"
"(def tuples (xs (o n 2))\n"
"\"Splits 'xs' up into lists of size 'n'. Generalization of [[pair]].\"\n"
"  (if (no xs)\n"
"    nil\n"
"    (cons (firstn n xs)\n"
"          (tuples (nthcdr n xs) n))))\n"
"\n"
"(def copylist (x) x)\n"
"\n"
"(def inc (x (o n 1))\n"
"  (coerce (+ (coerce x 'int) n) (type x)))\n"
"\n"
"(def range (start end)\n"
"\"Returns the list of integers from 'start' to 'end' (both inclusive).\"\n"
"  (if (> start end)\n"
"    nil\n"
"    (cons start (range (inc start) end))))\n"
"\n"
"(mac n-of (n expr)\n"
"  \"Runs 'expr' 'n' times, and returns a list of the results.\"\n"
"  (w/uniq ga\n"
"    `(let ,ga nil\n"
"       (repeat ,n (push ,expr ,ga))\n"
"       (rev ,ga))))\n"
"\n"
"(def counts (seq (o tbl (table)))\n"
"\"Returns a table with counts of each unique element in 'seq'.\"\n"
"  (let ans tbl\n"
"    (each x seq\n"
"	  (++ (ans x 0)))\n"
"    ans))\n"
"\n"
"(def compare (comparer scorer)\n"
"  \"Creates a function to score two args using 'scorer' and compare them using\n"
"'comparer'. Often passed to [[sort]].\"\n"
"  (fn (x y) (comparer scorer.x scorer.y)))\n"
"\n"
"(def commonest (seq)\n"
"  \"Returns the most common element of 'seq' and the number of times it occurred\n"
"in 'seq'.\"\n"
"  (withs (counts* (counts seq)\n"
"          best* (best (compare > counts*) seq)) \n"
"    (list best* (counts* best* 0))))\n"
"\n"
"(def retrieve (n f xs)\n"
"\"Returns the first 'n' elements of 'xs' that satisfy 'f'.\"\n"
"  (if (no n)                 (keep f xs)\n"
"      (or no.xs (<= n 0))    nil\n"
"      (f car.xs)             (cons car.xs (retrieve (- n 1) f cdr.xs))\n"
"                             (retrieve n f cdr.xs)))\n"
"(def most (f seq)\n"
"\"Like [[best]], but function 'f' is a scorer for each element rather than a\n"
"comparator between elements.\"\n"
"  (if seq\n"
"    (withs (wins (car seq) topscore (f wins))\n"
"      (each elt (cdr seq)\n"
"        (let score (f elt)\n"
"          (if (> score topscore) (= wins elt topscore score))))\n"
"      wins)))\n"
"\n"
"(def mem (test seq)\n"
"\"Returns suffix of 'seq' after the first element to satisfy 'test'.\n"
"This is the most reliable way to check for presence, even when searching for nil.\"\n"
"  (let f (testify test)\n"
"    (reclist [if (f:carif _) _] seq)))\n"
"\n"
"(def insert-sorted (test elt seq)\n"
"\"Inserts 'elt' into a sequence 'seq' that is assumed to be sorted by 'test'.\"\n"
"  (if (no seq)\n"
"       (list elt)\n"
"      (test elt car.seq)\n"
"       (cons elt seq)\n"
"      'else\n"
"      (cons car.seq (insert-sorted test elt cdr.seq))))\n"
"\n"
"(mac insort (test elt seq)\n"
"  \"Like [[insert-sorted]] but modifies 'seq' in place'.\"\n"
"  `(zap [insert-sorted ,test ,elt _] ,seq))\n"
"\n"
"(def reinsert-sorted (test elt seq)\n"
"  (if (no seq)\n"
"       (list elt)\n"
"      (is elt car.seq)\n"
"       (reinsert-sorted test elt cdr.seq)\n"
"      (test elt car.seq)\n"
"       (cons elt (rem elt seq))\n"
"      'else\n"
"       (cons car.seq (reinsert-sorted test elt cdr.seq))))\n"
"\n"
"(mac insortnew (test elt seq)\n"
"  \"Like [[insort]], but only inserts 'elt' if it doesn't exist.\"\n"
"  `(zap [reinsert-sorted ,test ,elt _] ,seq))\n"
"\n"
"(def bestn (n f seq)\n"
"  \"Returns a list of the top 'n' elements of 'seq' ordered by 'f'.\"\n"
"  (firstn n (sort f seq)))\n"
"\n"
"(def count (test x)\n"
"\"Returns the number of elements of 'x' that pass 'test'.\"\n"
"  (with (n 0 testf testify.test)\n"
"    (each elt x\n"
"      (if testf.elt ++.n))\n"
"    n))\n"
"\n"
"(def union (f xs ys)\n"
"\"Merges 'xs' and 'ys', while filtering out duplicates using 'f'. Ordering is\n"
"not preserved.\"\n"
"  (+ xs (rem (fn (y) (some [f _ y] xs))\n"
"             ys)))\n"
"\n"
"(def len< (x n)\n"
"  \"Is [[len]] of 'x' less than 'n'?\"\n"
"  (< len.x n))\n"
"\n"
"(def len> (x n)\n"
"  \"Is [[len]] of 'x' greater than 'n'?\"\n"
"  (> len.x n))\n"
"\n"
"(def dedup (xs)\n"
"\"Returns list of elements in 'xs' with duplicates dropped.\"\n"
"  (let h (table)\n"
"    (accum yield\n"
"      (each x xs\n"
"        (unless h.x\n"
"          (yield x)\n"
"          (set h.x))))))\n"
"\n"
"(def single (x)\n"
"\"Is 'x' a list with just one element?\"\n"
"  (and acons.x (no cdr.x)))\n"
"\n"
"(def before (x y seq (o i 0))\n"
"\"Does 'x' lie before 'y' in 'seq' (optionally starting from index 'i')?\"\n"
"  (aand (pos (orf testify.x testify.y) seq i)\n"
"        (iso x seq.it)))\n"
"\n"
"(def rand-elt (seq)\n"
"\"Returns a random element of 'seq'. See also [[rand-choice]].\"\n"
"  (seq (rand (len seq))))\n"
"\n"
"(mac point (name . body)\n"
"\"Like [[do]], but may be exited by calling 'name' from within 'body'.\"\n"
"  `(ccc (fn (,name) ,@body)))\n"
"\n"
"(mac catch body\n"
"\"Runs 'body', but any call to (throw x) immediately returns x.\"\n"
"  `(point throw ,@body))\n"
"\n"
"(def mismatch (s1 s2)\n"
"\"Returns the first index where 's1' and 's2' do not match.\"\n"
"  (catch\n"
"    (on c s1\n"
"      (when (isnt c (s2 index))\n"
"        (throw index)))))\n"
"\n"
"(def sum (f xs)\n"
"\"Returns total of all elements in (map1 f xs).\"\n"
"  (let n 0\n"
"    (each x xs\n"
"	  (++ n f.x))\n"
"    n))\n"
"\n"
"(mac rand-choice exprs\n"
"\"Runs one of the given 'exprs' at random and returns the result.\"\n"
"  `(case (rand ,(len exprs))\n"
"     ,@(let key -1\n"
"         (mappend [list (++ key) _]\n"
"                  exprs))))\n"
"\n"
"(def only (f)\n"
"\"Transforms a function 'f' info a variant that runs only if its first arg is\n"
"non-nil.\"\n"
"  (fn args (if (car args) (apply f args))))\n"
"\n"
"(mac summing (sumfn . body)\n"
"  \"Sums the number of times sumfn is called with a true argument in body. The sum is returned. The sumfn argument specifies the name under which the summing function is available to the body.\"\n"
"  (w/uniq gacc\n"
"    `(withs (,gacc 0 ,sumfn [if _ (++ ,gacc)])\n"
"       ,@body\n"
"       ,gacc)))\n"
"\n"
"(def map (proc . arg-lists)\n"
"  (if (and (car arg-lists) (all idfn (map1 car arg-lists)))\n"
"      (cons (apply proc (map1 car arg-lists))\n"
"            (apply map (cons proc\n"
"                             (map1 cdr arg-lists))))\n"
"      nil))\n"
"\n"
"(def intersperse(x ys)\n"
"	\"Inserts 'x' between the elements of 'ys'.\"\n"
"	(and ys(cons(car ys)\n"
"	(mappend[list x _](cdr ys)))))\n"
