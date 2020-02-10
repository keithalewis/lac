Don't special case things!

read a token, look it up, execute

## Parsing

`lac` uses a file stream to parse white space delimited characters into tokens.

When a regular token is encountered its symbol is looked up in the dictionary.  
The number and type of arguments required by the symbol are used to parse the required arguments from the stream. 
If a newline is encountered before the required number of arguments are seen the remaining arguments
are popped from the stack.
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
chars -> tokens -> thunks

              local   global
environment = stack x dictionary

character stream -> token stream

stack as a token stream

concatenate token streams

## Examples

```
double to_double(const variant a)
{
	return *(double*)lac_variant_address(&a);
}
```

```
#define VARIANT_TO_TYPE(T, V) *(T*)lac_variant_address(&V)
```

variant add(const variant a, const variant b)
{
	variant c;

	if (&ffi_type_float == a.type) {
	}

	return c;
}

```
: int fib -- int {
	<= 2 @ ? ! int 1 break
	+ - @ 1 - @ 2
}
```

```
: int gcd -- int int {
}
```
