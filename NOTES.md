Don't special case things!

read a token, look it up, execute

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
