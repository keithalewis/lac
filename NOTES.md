Don't special case things!

read a token from input stream, look it up, execute

## Parsing

`lac` uses a file stream to parse white space delimited characters into tokens.

When a regular token is encountered its symbol is looked up in the dictionary.  
The number and type of arguments required by the symbol are used to parse the required arguments from the stream. 
If a newline is encountered before the required number of arguments are seen the remaining arguments
are popped from the stack. (This is a special case!)
The function corresponding to the symbol is then called on the arguments and the result is pushed on the stack.  

## Special Tokens

Some tokens receive special processing.

### Stack Items

Use `@<n>` to pick the n-th item from the stack.  
Use `!<n>` to pick the n-th item from the stack and remove it.  


### Strings

Tokens starting with double quote (`"`) are processed until the next double quote.
Double quotes in a string preceded by a backslash (`\`) are escaped.


### Blocks

Tokens starting with left bracket ('{') are processed until the next right bracket at
the same nesting level is encountered.

### Variables

Tokens starting with colon (`:`) define variables that are added to the dictionary.  

### Comments

Comments start with an octothorpe (`#`) and end with the next newline character.
Octothorpes in a string preceded by a backslash (`\`) are escaped.


# Unfiled

          parse    compile
char stream -> tokens -> thunks

              local   global
environment = stack x dictionary

character stream -> token stream

stack as a token stream

concatenate token streams

constant thunk? 

Use library as namespace? m:cos calls cos from libm.so

## Examples

```
variant add(const variant a, const variant b)
{
	variant c;

	if (&ffi_type_float == a.type) {
		if (&ffi_type_xxx == b.type) {
		}
	}

	return c;
}

```
# fib n = fib n-1 + fib n-2
: int fib int { # -- n
	: n ! # redefine ":" to use local stack dictionary
	? <= @ 2 int 1 break
	decr ! 1 # -- n-1
	decr @ 1 ## -- n-2 n-1
	add fib ! 1 fib @ 1
	# remove stack dictionary
}
```

```
: int gcd -- int int {
}
```

Primary functions: `dlopen`, `dlsym`, `dlclose`, `load`, `loadv`, `unload`, `call`, `callv`

Use &xxx to get ffi_type_xxx address

Use '(' to store stack depth and ')' to assert it is the same as last matching '('

```
: cos load &double ( dlsym dlopen libm.so.6 cos ) &double
```

```
: key val
```

## wc

: l int 0 # char* "l" -> void* &lac_variant{value._sint = 0, .type = &ffi_type_sint}
: w int 0
: fp fopen file.txt r
: c fgetc fp
loop {
	if == c EOF break
	incr c
	if isspace c incr w
	if == c '\n' incr l
	: c fgetc fp
}
printf "%d %d %d" l w c
fclose fp

## Use va_args? No! It would have types x nargs cases

token = lac_parse_token(stream)

lac_variant lac_call_token(stream, token)
{
	thunk = get(token)
	ensure thunk // token must refer to a cif
	return lac_call_thunk(fp, thunk)
}

lac_variant lac_call_thunk(fp, thunk)
{
	lac_variant v

    n = thunk->nargs
	lac_variant args[n]
	void* addr[n]
	for (i = 0..n) {
		char* tok = lac_parse_token(fp)
		args[i] = lac_evaluate_token(fp, tok)
		addr[i] = address(args[i])
	}
	lac_cif_call(thunk, &result, addr)

	return v;
}

lac_variant lac_evaluate_token(fp, tok)
{
	lac_variant v;

	if (*tok == ") {
	}
	else if (*tok = {) {
	}
	else if (thunk = get(tok)) {
		v = lac_call_thunk(fp, thunk)
	}
	else {
		scan v
	}

	return v;
}

use intptr_t to switch on ffi_type pointers

evaluate(is, dict) -- look in dict first, then global dict

void loop_(const lac_variant block)
{
	ensure (block.type == &ffi_type_block);
	const char* s =  lac_variant_address(block);
	size_t n = strlen(s);
	// ???local dictionary???
	// set break = false
	// set continue = false
	while (true) {
		FILE* is = fmemopen(s, n, "r");
		evaluate(is);
		fclose(is);
		if (break)
			break;
		if (continue)
			continue;
	}
}

eval(is, dict) -> variant
	token = next(is)
	variant = get(token, dict)
	if not found
		return token
	if cif
		return call(variant, is, dict)
	if block
		return eval(fmemopen(block), dict)
	return variant


