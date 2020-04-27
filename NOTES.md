Don't special case things!

convert string to fd: memfd_create and write the string (use lseek?)

_ var val
_var -> value

prog: 

read a token from input stream
look it up
    if cif execute
    else value
    free token

cif execute
    foreach arg
        read token from stream
            argi = eval_type
    call function
    foreach arg
        free arg

## Parsing

https://web.archive.org/web/20090824073244/http://pplab.snu.ac.kr/courses/adv_pl05/papers/p261-knuth.pdf

Ole-Johan Dahl syntax.

`loop; S; while ~B: T; repeat;`

```
                   {P}S{Q}
               {Q and ~B}T{P}
-------------------------------------------
{P} loop: S; while B: T; repeat; {Q and ~B} 
```
loop {S} while {!B} {T} repeat
loop while {!B} {T} repeat
loop {S} while {!B} repeat
loop {S} repeat // break and continue?

proc # { loop while != '\n' getc repeat }

if {B} then {T} else {F}
if {B} then {T}

for {C} {S} // for {var i iota} { print i }


Use backtick to execute shell command in child and open pipe to read.

`token_read` fd -> token

`scan_token` token -> type

`eval_token`

## Token

A _token_ is a string of characters. String. Block.

Use a dictionary stack.
Push new dict on call, pop on exit.
Variables are shadowed.

Associate dict stack with cif.

---

Use file descriptors to accomodate sockets and pipes.
Use coroutines - https://gist.github.com/laindir/6369535

Use proc/coro to define functions.

token* pt = token_read(i)
token {
    char type // first character
    size_t size
    char data[]
}
token_free(pt) to free data

// read token and free if needed
object* po = object_parse(type, i);
object {
    lac_type type; // int, ..., pointer, cif, string, block
    /*
    int (*print)(int); // decode to file descriptor
    int (*parse)(object*, int); // encode from file descriptor
    object* (*make)(lac_type, size_t);
    void (*copy)(const object*); // assign from object
    void (*free)(object*); // based on type
    */
    size_t size // use size = 0 to indicate data should not be freed
    char data[8] // 8 bytes to hold scalars?
}
object_free(po)
object_print(po, o) { print(o, po->data, po->size); }
void* object_addr returns &data[0];

// type maps
int (*print)(int) type_print(type);
...
void (*free)(object*) type_free(type);

put var obj // add var to dictionary
getp var // &var - pointer to var
getv var // *var - value of var

void object_print(obj, o) // send to output stream

All objects require a parse and print function.
All objects require a new and del function.

Every statement starts with a cif.
The cif parses the arguments from the input stream based on arg types
and calls the function.

object* eval(po, i)
{
    switch (po->type) {
		case cif:
			return call(po, i)
			break;
	}
}

Use dict for RAII?
When dict goes out of scope free all values

Use 'set' for immutable values???

```
wc.lac

let i fdopen 0 r
let c getc i
while != *c EOF {
	set skip false
	incr &c
	if and skip isspace *c {
		incr &w
		set skip true
	}
	else {
		let skip false
	}
	if == *c '\n' {
		incr &l
	}
	let c getc i
}

```
struct tm { 
    sint tm_sec
    ...
    sint tm_isdst
}


```

```

int parse_sint(int is, lac_object** po)
{
    *po = lac_object_make(
    FILE* fs = fdopen(is, "r");
    int n = fscanf(fs, "%d", *po->data);

    return n;
}

```

