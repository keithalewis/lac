# Load And Call

Load and call C functions at runtime.

This little language makes it possible to dynamically load C
functions from shared libraries at runtime and call them using
[libffi](https://github.com/libffi/libffi).

If the `puts` function from the standard library has
been **load**ed
```
puts "Hello World!"
```
will print `Hello World!` followed by a newline character
and return an integer value, as documented in
the [puts(3)](http://man7.org/linux/man-pages/man3/puts.3.html)
man page.

String _tokens_ are looked up in the _dictionary_ and the corresponding
C function is *call*ed. The `puts` function requires a string to be the
next token on the input stream.  `Lac` uses double quotes for strings
that may contain spaces. The result of the call is an integer value.

An equivalent way to do this is
```
puts Hello\ World!
```
since backslash (`\`) can be used to escape white space characters on the
input stream.

The `puts` function from the C standard library can be placed
in the dictionary using
```
_ puts load sint ( dlsym ( dlopen libc.so ) puts ) pointer void
```
The _underscore_ (`_`) function is used to place a _key_ and a _value_ into
the dictionary. 

Use `load` to specify the _signature_ of the pointer returned by
`dlsym` to create a _cif_: a C InterFace. The signature specifies
the return type of the function, a pointer to a C function, and the
required arguments. The required arguments are terminated by `void`.
The type `void` is not a valid argument type and indicates the end of
the arguments.

The functions `dlopen`, `dlsym`, and `dlclose` from `<dlfcn.h>` are
preloaded. 

The standard C types are also preloaded. The token `sint` indicates
the type of a signed int that is native to the platform. The `pointer`
type is a `void*` pointer.

The parentheses are not required since `dlsym` knows it needs two
arguments, a pointer returned by `dlopen` and the string name of
the function. Likewise, `dlopen` knows it only needs one argument,
the string name of the library.

Parentheses can be used not just for readability; the closing parentheses
ensures all required arguments have been supplied.

An equivalent way to do the above is
```
_ puts ( load int dlsym dlopen libc.so puts string )
```
The tokens inside the parentheses are evaluated to a cif
that is associated with the key `puts` in the dictionary.

## Evaluation

`Lac` _evaluates_ an _input stream_ of white space
separated string tokens and returns the _required type_. 
It evaluates the input stream until no more tokens are found.

If the required type is not a string then its _value_ is looked up in
the dictionary.  Values can be any of the standard C integal types,
floating point numbers, or pointers.

If the value is a `cif` then it evaluates the input stream based on its
required argument types and calls the corresponding C function to produce
the required type.  This must match the return value of the function.

If the value is not a `cif` its type must must match the required type.

If the value is not found in the dictionary then the string token is
parsed based on the required type.

If a token starts with a backtick character (`'`'`) then it is
not looked up in the dictionary and the backtick is removed.
The required type must be a string.

### Tokens

A `lac` program parses a stream of characters from a file into _tokens_.
Tokens are separated by white space according to `isspace`.

A _string_ token starts with a quote character (`'"'`) and all characters
are included up to the next quote character. Quote characters preceded by
a backslash (`'\'`) get included in the token.

A string token does not include the beginning and ending quote characters.  

A _block token_ starts with a left brace character (`'{'`) and all
characters are included up to the next right brace character (`'}'`) at
the same nesting level.  Right brace characters preceded by a  backslash
(`'\'`) get included in the token.

A block token does not include the beginning and ending brace characters.

### Types

The scalar types used by `libffi` are `schar`, `uchar`, `sshort`, `ushort`,
`sint`, `uint`, `slong`, `ulong`, `float`, `double`, `sint8`, `uint8`,
`sint16`, `uint16`, `sint32`, `uint32`, `sint64`, `uint64`, and
`pointer`.

The `variant` type is a union of these types and a field indicating the type.

The types `string` and `block` are pointer types where the
pointer is a null terminated string of characters.

A `cif` type is a pointer type that has a C function symbol,
a `ffi_cif` member from `libffi` describing the C InterFace,
and an array containing the argument types required by `ffi_cif`.

Functions can have return type `void` but `void` cannot be used as an
argument type.  Functions taking no arguments have an empty array of
argument types.

### Dictionary

The _dictionary_ has strings as keys and variants as values.
Use `: key value` to add items to the dictionary.

### Built-in Types

We've see the built-in types, `dlopen`, `dlsym`, `dlclose`, `:`,
`(`, `)` and all the standard C types. There are also the
special types `string`, `block`, `variant` and `cif`.

The hash character (`#`) is another built-in function. It
discards all characters on the input stream up to the next
newline character.

### Control Flow

Use `loop {block}` to repeatedly call _block_

## Examples

This program reads `stdin` and writes to `stdout`.
It uses the predefined `FILE*` pointers `stdin` and `stdout`
and the predefined signed integer `EOF`.

```
: getc ( load int ( dlsym ( dlopen libc.so ) getc ) pointer )
: putc ( load int ( dlsym ( dlopen libc.so ) putc ) int pointer )
: c getc stdin
while { not equal c EOF } {
	putc c stdout
	: c getc stdin
}
```

This is toy version of `wc(1)`.

```
: l ( parse int 0 )
: w parse int 0
: fp fopen filetxt r
: c fgetc fp
loop {
	if equal c EOF break
	incr c
	if isspace c incr w
	if equal c '\n' incr l
	: c fgetc fp
}
printf "%d %d %d\n" l w c
fclose fp
```
