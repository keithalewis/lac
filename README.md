![Build Status](https://github.com/keithalewis/lac/workflows/C/C++%20CI/badge.svg)

# Load And Call

Load and call C functions at runtime.

This simple language makes it possible to dynamically load C
functions from shared libraries at runtime and call them using
[libffi](https://github.com/libffi/libffi).

If the [`puts`](http://man7.org/linux/man-pages/man3/puts.3.html)
function from the C standard library has
been **load**ed
```
puts "Hello World!"
```
will print `Hello World!` on the standard output stream followed by
a newline character and return an integer value, exactly as described in
the man page.

`Lac` reads _tokens_ from standard input.
When it sees `puts` it tries to find a _dictionary_ entry with that key.
The value in the dictionary knows the C symbol for `puts` from
the C standard library and its signature `int puts(const char*)`.

`Lac` then reads the token `Hello World!`, **call**s `puts` with a
pointer to that string, and returns an `int` value.

This continues until there are no characters remaining on the input stream.

`Lac` is a simple language. It takes inspiration from [Tcl](https://www.tcl-lang.org/)
and uses `libffi` to obviate the need to compile code.

## Streams

Lac uses file descriptors for input, output, and error streams.
By default these are `stdin`, `stdout`, and `stderr`.
These can be redirected using syntax similar to the unix shell.
The token `<` replaces `stdin` with the file named by the following token
and `>` replaces `stdout`similarly. Use `>&` to duplicate the current
output.

## Types and Values

A (_value_) _type_ describes the bits of its possible _value_s.
Every type has a _parse_ function for converting an _input stream_ of
characters into the bits of the type and a _print_ function to serialize
the bits back to a stream of characters.

The parse function allocates the bits associated with a type. Every type
has a _free_ function that deallocates these bits.

Every value also has a _copy_ function that creates a new value of the same
type with identical bits. The _same_ function takes two values and
returns `true` if and only if they are the same type and have the same bits.

`Lac` knows about all the standard C types.  The scalar types are `schar`,
`uchar`, `sshort`, `ushort`, `sint`, `uint`, `slong`, `ulong`, `float`,
`double`, `sint8`, `uint8`, `sint16`, `uint16`, `sint32`, `uint32`,
`sint64`, `uint64`, and `pointer`. Users can define types for structures
and unions using the `struct` and `union` commands.

There are also types for C functions (`func`). It has the corresponding
C symbol and its _signature_. The signature is its return type and
arguments types.  The corresponding value is created using `bind`
by providing the values of the argument types. The `call` function
evaluates the function using these arguments and returns the value of
the result type.

## Dictionaries

A dictionary contains key-value pairs where keys are strings.
Pairs are added to a dictionary using `set key value`,
retrieved using `get key`, and removed using `del key`.

Scoping and object lifetime is performed using `{` and `}`. 
The function `{` pushes a new dictionary on the top of the stack
and `}` deletes all entries added since the previous matching `{`.

Key lookup is performed by searching the dictionary stack. If there
are duplicate keys then the most recent value is returned.

(Possible to return dictionaries?)
(Possible to use dictionaries as arguments?)

## Evaluation

`Lac` reads space separated tokens from the input stream and converts
them to values based on the required type.

If a token starts with a quote (`"`) or left brace character (`{`) then it
is not looked up in the dictionary.  The required type must be a string.

If the token is in the dictionary and its value is a cif then the
arguments required by the cif are evaluated from the stream, the function
is called, and the result is the return type of the function.

If the value is not a function the result is the dictionary value.

If the token is not in the dictionary then it is parsed to a value
based on the required token type.

This continues until there are no characters remaining on the input stream.

## Load

The `puts` function from the C standard library can be placed
in the dictionary using
```
load puts sint ( dlsym ( dlopen libc.so ) puts ) pointer void
```
The `load` function is used to place a _key_ and a _value_ into
the dictionary. 

Note the _signature_ of the pointer returned by
`dlsym` to create a cif. The signature specifies
the return type of the function, a pointer to a C function, and the
required arguments. The required arguments are terminated by `void`.
The type `void` is not a valid argument type and indicates the end of
the arguments.

The functions `dlopen`, `dlsym`, and `dlclose` from `<dlfcn.h>` are
preloaded. 

The standard C types are also preloaded. The token `sint` indicates
the type of a signed int that is native to the platform. The `pointer`
type is a `void*` pointer. `Lac` has no notion of `const`.

The parentheses are not required since `dlsym` knows it needs two
arguments, a pointer returned by `dlopen` and the string name of
the function. Likewise, `dlopen` knows it only needs one argument,
the string name of the library.

Parentheses can be used not just for readability; the closing parentheses
ensures all required arguments have been supplied.

An equivalent way to do the above is
```
load puts ( int dlsym dlopen libc.so puts string )
```

## Data Structures

Every type is associated with a unique small integer.
This is used as an index into arrays for parsing, printing,
allocating, copying, and freeing values of that type.

---

New functions to be interpreted by `lac` can be defined by
specifying their name, signature, and body.
```
proc skipws void {
    : c getc
    while isspace c {
        : c getc
    }
} void
```


### Tokens

A `lac` program parses a stream of characters from a file into _tokens_.
Tokens are separated by white space according to `isspace` or the
end of the input stream.

A _string_ token starts with a quote character (`'"'`) and all characters
are included up to the next quote character. Quote characters preceded by
a backslash (`'\'`) get included in the token.

A string token does not include the beginning and ending quote characters.  

A _block token_ starts with a left brace character (`'{'`) and all
characters are included up to the next right brace character (`'}'`) at
the same nesting level.  Left and right brace characters preceded by a  backslash
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

This program reads `stdin` and prints to `stdout`.
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

```
proc sint foo {double} {
	return cmp x x
}
```
```
_ puts int (dlsym ...) {double sint}
```
```
_ printf int (dlsym ...) {string ...}
```

The dictionary is a stack. Local variables are push on it and popped
at the end of a function.

```
: puts (...) int pointer void
```
Lable and goto using setjmp longjmp.

```
parse sint 123
write sint 123
```
