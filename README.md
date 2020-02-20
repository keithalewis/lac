# Load And Call

Load and call C functions at runtime.

This little language makes it possible to dynamically load C
functions from shared libraries at runtime and call them using
[libffi](https://github.com/libffi/libffi).

The functions `dlopen`, `dlsym`, and `dlclose` from `<dlfcn.h>` are
available to get a pointer to any C function from a dynamic library.

Use `load` to specify the signature of the function and `call` to call it
with the specified signature.

A _dictionary_ is available for associating keys with values.
Use `> key value` to put `value` in the dictionary and `< key` to retrieve it.

## Parse

A `lac` program parses a stream of characters from a file into _tokens_.
Tokens are seperated by white space according to `isspace`.

If a token starts with a quote character (`'"'`) then white space is
included up to the next quote character. Quote characters 
preceded by a backslash (`'\'`) get included in the token.

The string token does not include the beginning and ending quote characters.

A _block token_ starts with a left brace character (`'{'`) and all characters
are included up to the next right brace character (`'}'`) at the same nesting level.
Right brace characters preceded by a  backslash (`'\'`) get included in the token.

The block token does not include the beginning and ending brace characters.

## Execute

A `lac` program looks up the first token as a key
in the dictionary and executes the corresponding function. The function knows what
arguments it needs, evaluates required tokens from the stream, then calls
the function.

## Evaluate

If a required token is in the dictionary then it is executed and the resulting type
must match what is required by the function signature.

If not it is converted to the type required by the function using `sscanf`.

### Types

The types used by `libffi` are `schar`, `uchar`, `sshort`, `ushort`, `sint`,
`uint`, `slong`, `ulong`, `float`, `double`, `sint8`, `uint8`,
`sint16`, `uint16`, `sint32`, `uint32`, `sint64`, `uint64`,
and `pointer`. Functions can have return type `void`.

The prefix `s` means signed and `u` means unsigned. Types not involving
integers correspond to their machine dependent types.

## Predfined Dictionary Entries

All of the above types can be used to turn the next token on the input stream
into the corresponding type. If you leave off the prefix the corresponding
type is assumed to be signed.

The predefined `>` function only knows that the next token is a string.

## Example

This is toy version of `wc(1)`.

```
> l int 0 # char* "l" -> void* &lac_variant{value._sint = 0, .type = &ffi_type_sint}
> w int 0
> fp fopen file.txt r
> c fgetc fp
loop {
	if == c EOF break
	incr c
	if isspace c incr w
	if == c '\n' incr l
	: c fgetc fp
}
printf "%d %d %d" l w c
fclose fp
```
