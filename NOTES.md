Get rid of dbm. Use chunk of memory and use string -> pointer table for lac_cif.

thunk - lac_cif = ffi_cif + sym + arg_types
closure - thunk + partial args

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

:skip_space { -- FILE* - FILE*
	loop {
		fgetc @ -- c FILE*
		== EOF ? break
	}
}

:l int 0 -- nothing
:w int 0
:c int 0
fopen file.txt r -- FILE*
fgetc @ -- c FILE*
loop {
	== EOF @ ? break
	incr &c
	== '\n' @ ? incr &l -- c FILE*
	isspace ? incr &w skip_space @ -- FILE*
	fgetc @ -- c FILE*
}
