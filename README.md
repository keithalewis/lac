# Load And Call

Load and call C functions at runtime.

This little language makes it possible to dynamically load C
functions from shared libraries at runtime and call them using
[libffi](https://github.com/libffi/libffi).

The functions `dlopen`, `dlsym`, and `dlclose` from `<dlfcn.h>`
can be used to get a pointer to a C function from a dynamic library.

Use `load` to specify the signature of the function returned by `dlsym`
so `call` can parse required argumnets from the input stream.

A _dictionary_ is available for associating keys with values.

## Tokens

A `lac` program parses a stream of characters from a file into _tokens_.
Tokens are separated by white space according to `isspace`.

If a token starts with a quote character (`'"'`) then white space is
included up to the next quote character. Quote characters preceded by
a backslash (`'\'`) get included in the token.

The string token does not include the beginning and ending quote characters.  A _block token_ starts with a left brace character (`'{'`) and all
characters are included up to the next right brace character (`'}'`) at
the same nesting level.  Right brace characters preceded by a  backslash
(`'\'`) get included in the token.

The block token does not include the beginning and ending brace characters.

## Types

The scalar types used by `libffi` are `schar`, `uchar`, `sshort`, `ushort`,
`sint`, `uint`, `slong`, `ulong`, `float`, `double`, `sint8`, `uint8`,
`sint16`, `uint16`, `sint32`, `uint32`, `sint64`, `uint64`, and
`pointer`.

The `variant` type is a union of these types and a field indicating the type.

The types `string` and `block` are pointer types where the
pointer is a null terminated string of characters that have been allocated
on the heap.

A `cif` type is a pointer type that has a C function symbol,
a `ffi_cif` member from `libffi` describing the C InterFace,
and an array containing the argument types required by `ffi_cif`.

Functions can have return type `void` but `void` cannot be used as an
argument type.  Functions taking no arguments have an empty array of
argument types.

# Dictionary

The _dictionary_ has strings as keys and variants as values.
Use `: key value` to add items to the dictionary.

The dictionary is populated with certain well known keys.

The colon string `":"` is a dictionary key. It corresponds
to a cif that looks for a string and a variant on the
input stream and adds them to the dictionary.

Every type is mapped to its corresponding `libffi` type.
E.g., the `"double"` key corresponds to a pointer variant with
value `&ffi_type_double`.

The `"parse"` entry consumes a type and a string and uses
`sscanf` to produce a variant of that type.

## Parsing

A `lac` program reads characters from an input stream and parses
them into tokens. Every token is either a string or block.
String tokens are looked up in the dictionary and are replaced
by their variant value. If the string token is not in the
dictionary no action is taken.

If the variant is a `cif` the corresponding
function is called, parses the required arguments, then calls
the function. The result is the variant corresponding to the
return type of the function.

## Load

The `load` function looks for the return type, a pointer to a function, and
a list of argument types that terminated by `void` on the input stream.
It returns a `cif` type.

If an ellipsis `...` is encountered then the function is
variadic.

## Call

The `call` function looks up the next token in the dictionary. It must be
a cif and it consumes required tokens from the input stream, calls
the corresponding C function on these arguments, and returns a variant
of the specified return type.

If the cif is variadic the optional arguments are terminated using `void`.



## Execute

A `lac` program looks up the first token as a key in the dictionary and
executes the corresponding function. The function knows what arguments it
needs, evaluates required tokens from the stream, then calls the function.

## Evaluate

If a required token is in the dictionary then it is executed and the
resulting type must match what is required by the function signature.

If not it is converted to the type required by the function using `sscanf`.
## Predefined Dictionary Entries

All of the above types can be used to turn the next token on the input stream
into the corresponding type. If you leave off the prefix the corresponding
type is assumed to be signed.

The predefined `>` function only knows that the next token is a string.




execute 
	token = parse_token(stream)
	variant = lookup_variant(token)

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
