![Build Status](https://github.com/keithalewis/lac/workflows/C/C++%20CI/badge.svg)

# Load And Call

Load and call C functions at runtime.

This simple language makes it possible to dynamically load C
functions from shared libraries at runtime and call them using
[libffi](https://github.com/libffi/libffi).

If the `puts` function from the C standard library has
been **load**ed
```
puts "Hello World!"
```
will print `Hello World!` on the standard output stream followed by
a newline character and return an integer value.

An equivalent way to do this is
```
puts Hello\ World!
```
since backslash (`\`) can be used to escape white space characters on the
input stream.

`Lac` reads space seperated tokens from standard input.
When it sees `puts` it tries to find a _dictionary_ entry with that key.
The value in the dictionary has the C symbol for `puts` from
the C standard library and its signature `int puts(const char*)`.

`Lac` then tries to read the required argument as a `const char*` from
the input stream.  If that succeeds then `puts` is **call**ed with that
argument and returns an `int` value.

This continues until there are no characters remaining on the input stream.

`Lac` is a simple language.

## Dictionary

The dictionary contains key-value pairs. Keys are strings and values
are `void*` pointers to any C type. Lac knows about the usual C
integral, floating point, and `void*` types. It uses a _variant_
type called `lac_variant` that is a union of these types together with
a `type` field to discriminate.

It also knows about a special type called _cif_, a C InterFace.
The `lac_cif` struct has a C symbol and its _signature_.  The signature
specifies the return type and the types of all required arguments.

`Lac` uses the dictionary to define variables. Once added to the
dictionary it is looked up as described above on any remaining input.

The dictionary can have multiple entries with the same key. The most
recently added key is found during lookup.

This is used to provide a form of lexical scoping for function call
stacks. Any variables added in the body of a function are removed
when the function returns.

## Evaluation

`Lac` reads space seperated tokens from the input stream and
looks them up in the dictionary.

If the corresponding dictionary value is a cif then its required arguments
are evaluated from the stream, the function is called, and the result
is the return type of the function.

If the value is not a function the result is the value.

If the token is not in the dictionary then it is parsed to a value
based on the required token type.

This continues until there are no characters remaining on the input stream.

If a token starts with a backtick character (`'`'`) then it is
not looked up in the dictionary and the backtick is removed.
The required type must be a string.

## Load

The `puts` function from the C standard library can be placed
in the dictionary using
```
: puts load sint ( dlsym ( dlopen libc.so ) puts ) pointer void
```
The _colon_ (`:`) function is used to place a _key_ and a _value_ into
the dictionary. 

Use `load` to specify the _signature_ of the pointer returned by
`dlsym` to create a cif. The signature specifies
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
: puts ( load int dlsym dlopen libc.so puts string )
```
The tokens inside the parentheses are evaluated to a cif
that is associated with the key `puts` in the dictionary.

New functions to be interpreted by `lac` can be defined by
specifying their name, signature, and body.
```
: skipws void {
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

The dictonary is a stack. Local variables are push on it and popped
at the end of a function.

```
: puts (...) int pointer void
```
