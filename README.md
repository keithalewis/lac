# Load And Call

Load and call C functions at runtime.

This little language makes it possible to dynamically load C
functions from shared libraries at runtime and call them using
[libffi](https://github.com/libffi/libffi).

For example, if the function `puts` from the standard library has
been **load**ed
```
<puts "Hello World!"
```
will print `Hello World!` (without quotes) and a trailing newline 
to standard output and return an integer.

The less than sign (`<`) indicates the key `puts` should be looked
up in the _dictionary_.

The `puts` function requires a string on the input stream
and `lac` uses double quotes for strings that may
contain spaces. The function is **call**ed with the string
argument and returns an integer.

An equivalent way to do this is
```
<puts Hello\ World!
```
since backslash (`\`) can be used to escape the
next character on the input stream.

The `puts` function from the C standard library can be placed
in the dictionary using
```
>puts <load <int ( <dlsym ( <dlopen libc.so ) puts ) <string <void
```
The _greater than_ (`>`) token is a function that places a key and a value into the dictionary.
Values are retrieved by using less than (`<`) in front of the key.
The similarity to shell file ouput and input (respectively) is
intentional. The key is the file name and the value is the
bits in the file. 

Use `load` to specify the _signature_ of the function returned by
`dlsym` to create a _cif_: a C InterFace. The signature specifies the
return type of the function, a pointer to a C function, and the required
arguments. The arguments are terminated by `void`. The syntax is
similar to the C function declaration. The type `void` is not
a valid argument type. 

The functions `dlopen`, `dlsym`, and `dlclose` from `<dlfcn.h>` are
preloaded functions that can be used to get a pointer to a C function
from a dynamic library.

All C types are also preloaded. E.g, the `string` type is a pointer type
to a null terminated string of characters.

The parentheses are not required since `dlsym` knows it needs two
arguments, a pointer returned by `dlopen` and the string name of
the function. Likewise, `dlopen` knows it only needs one argument,
the string name of the library.

They are used not just for readability; the closing parentheses
ensures all required arguments have been supplied.

An equivalent way to do the above is
```
>puts ( <load <int <dlsym <dlopen libc.so puts <string )
```

## Evaluation

`Lac` _evaluates_ an input stream of characters by reading white space
separated tokens.  

If a token has first character less than (`<`) it
is looked up in the dictionary.  If the value is a cif then it is called
and its return type is the value. Otherwise the dictionary value is used.

When a cif is called it knows the required argument types.  The input
stream is evaluted as above for each argument. If the value is a string
then it is parsed to the appropriate type.

If the token has first character greater than ('>') then the following
characters are used as a key in a new dictionary entry.
The rest of the stream is evaluated as above and the result is
the dictionary value.

This is a complete description of how `lac` evaluates input streams.

## Tokens

A `lac` program parses a stream of characters from a file into _tokens_.
Tokens are separated by white space according to `isspace`.

If a token starts with a quote character (`'"'`) then white space is
included up to the next quote character. Characters preceded by
a backslash (`'\'`) get included in the token.

The _string_ token does not include the beginning and ending quote characters.  

A _block token_ starts with a left brace character (`'{'`) and all
characters are included up to the next right brace character (`'}'`) at
the same nesting level.  Right brace characters preceded by a  backslash
(`'\'`) get included in the token.

The block token does not include the beginning and ending brace characters.

If a token starts with a less than character (`<`) the following characters
up to the next white space are a key in the dictionary.

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

The dictionary is populated with keys in addition to
`dlopen`, `dlsym`, and `dlclose`.

The colon string `":"` is a dictionary key. It corresponds
to a cif that looks for a string and a variant on the
input stream and adds them to the dictionary.

Every type is mapped to its corresponding `libffi` type.
E.g., the `"double"` key corresponds to a pointer variant with
value `&ffi_type_double` from `libffi`.

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
