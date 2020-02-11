# Load And Call

Load and call C functions at runtime.

This little language makes it possible to dynamically load C
functions from shared libraries at runtime and call them. It
uses [libffi](https://github.com/libffi/libffi).

## Load

```
load libm.so double cos double # load double cos(double) from libm.so
```
This adds `cos` to the _dictionary_ and whenever `cos` is a token in the
input stream after being loaded `lac` will call the cosine function.

The value associated with `cos` in the dictionary is a _thunk_: the C
function together with its signature. The signature is the return type
plus zero or more argument types.

The possible types are `int`, `float`, `double`, `uint8_t`, `int8_t`, `uint16_t`, `int16_t`,
`uint32_t`, `int32_t`, `uint64_t`, `int64_t`, and `void*`. Functions can have
return type `void` and push nothing on the stack after being called.

Variadic functions are loaded by specifying their fixed arguments followed
by an ellipsis (`...`). E.g.,
```
load libc.so int printf char* ...
```

## Call

Whenever a token is encountered in the input stream it is
looked up in the dictionary and the corresponding thunk is called to
consume the arguments it requires.

```
cos 0                 # call cos(0.) and push the result on the stack
```
Since `cos` knows it needs a `double` argument it parses the next token
using `strtod`, pushes it on the local call stack, calls the C function
`cos` with that argument, and pushes the result on the global stack as
a double.

The same result can be obtained by
```
cos double 0
```
When `cos` processes arguments it will look up `double` in the dictionary,
call the corresponding thunk, and push the result on the local call stack for `cos`.

Another way to call `cos(0.)` is
```
double 0
cos ;
```
The first line pushes 0 on the global stack as a double. When `;` is
encountered in the input stream it pops required arguments from the global stack onto the local call stack.


## Tokens

Tokens are white space delimited according to `isspace`. Note the newline character (`'\n'`) is white space.

### Strings

If a token starts with a double quote character (`'"'`) then white
space is included in the token until the next double quote character.
To include a `'"'` character in a string escape it with a backslash
`"like \"this\""` to get the string `like "this"`.

### Blocks

If a token starts with a left bracket character (`'{'`) then every character up to the
next right bracket (`'}'`) at the same nesting level is included in the token.
A right bracket can be escaped by preceding it with a backslash.

### Thunks

Use colon (`':'`) to add thunks to the dictionary.
```
: name thunk
```

Calling a thunk reads the input stream ...

If an argument is a token in the dictionary, it is evaluated and the result
is used as the argument.

## Predefined Thunks

Colon is a predefined thunk.

`#` &mdash; this thunk discards characters up to but not including the next
newline character. It can be used to include comments in the program text.

`-- type1 ...` &mdash; this thunk is used to verify the stack.  Execution
stops if the type arguments are not the same as the stack item types.

`@` &mdash; fetches the top item from the stack.

`!` &mdash; removes the top item from the stack.

`?` &mdash; consume the top item from the stack and execute what follows until next newline if non zero

`!?` &mdash; consume the top item from the stack and execute what follows until next newline if zero


## Example

This is toy version of `wc(1)`.

```
: l int 0 # lines
: w int 0 # words
: c int 0 # chars
fopen file.txt r -- FILE* # the top of the stack should be a file pointer
loop { # use break or continue for control flow
	fgetc @ -- int FILE*
	== EOF @ ? break # exit loop if end of file
	incr &c # increments the value pointed at by c
	isspace @ ? incr &w # ? consumes top of stack and executes what follows if non zero
	-- int FILE* # is a function that asserts the first and second stack items are an int and a FILE*
	# is a function that discards characters in the input stream up to the next newline
	== '\n' ? incr &l -- FILE* # == looks for its second argument on the stack and consumes it
	fgetc @ -- FILE* # get the next character from the file
}
! ! # pop int and FILE* off the stack even though that is not really needed
printf "%s %s %s\n" l w c # print lines, words, and characters
```
Words can be separated by multiple contiguous space characters so the above version overcounts words.
Once a space character is seen by the unix `wc` program any following white space is skipped. Define
```
:skip_space -- FILE* {
	loop {
		isspace fgets @ !? break # exit loop if character is not a space
	}
} -- FILE*
```
The line involving `isspace` could be replaced with
```
	isspace @ ? incr &w skip_space
```
except `skip_space` also skips newline characters. Use
```
:skip_space -- FILE* {
	loop {
		== '\n' fgets @ ? continue
		isspace fgets @ !? break # exit loop if character is not a space
	}
} -- FILE*
```
to avoid that.

The functions `--` and `#` are immediately executed when they are encounterd in the input stream.

## Unfiled

input stream
: a `FILE*` pointer

token
: a string from the input stream delimited by white space

type
: a C data type known to `lac`. The known types are ...

variant
: a data type that can hold any `lac` type

stack
: a FILO stack of variants

string
: a token beginning and ending with the double quote character (`'"'`).
Strings may contains white space charaters and the double quote
character can be escaped by preceding it with a backslash (`'\'`).

block
: a token beginning with left brace character (`'{'`) and ending
with a right brace character (`'}'`) at the same nesting level.  The right
brace character can be escaped by preceding it with a backslash (`'\'`).

thunk
: a C function together with its signature

dictionary
: a map from strings to thunks

type double type double ; # return type and arg
dlopen libm.so.6 RTDL_LAZY -- void*
dlsym cos # read handle from stack
: thunk lac_cif_alloc 1 -- cif* symbol* ret arg
lac_cif_prep 
```
