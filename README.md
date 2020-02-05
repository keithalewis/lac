# Load And Call

Load and call C functions at runtime.

This little language makes it possible to dynamically load C
functions from shared libraries at runtime and call them.
```
-lm double cos double # load double cos(double) from libm.so/.dll
```
will load the symbol `cos` from the C math library and add it to the _dictionary_. 
Whenever `cos` is a token in the input stream after being loaded `lac` will
call the cosine function.
```
cos 0                 # call cos(0.) and push the result on the stack
```
Another way to call `cos(0.)` is

```
double 0
cos ;
```

The first line pushes 0 on the stack as a double. When `cos` is encountered in the input
stream it will consume 0 from the stack since the semicolon (`;`) indicates there are no following arguments.


The possible stack types are `int`, `float`, `double`, `uint8_t`, `int8_t`, `uint16_t`, `int16_t`,
`uint32_t`, `int32_t`, `uint64_t`, `int64_t`, and `void*`.

Tokens are separated by whitespace using `isspace` from `<ctypes.h>`.

In general, whenever a token is encountered in the input stream it is looked up in the dictionary
and called to consume a known number of arguments. The special character semicolon (`;`) indicates
to the parser it should stop looking for arguments on the input stream and pop the rest of
the required arguments from the stack.

## Strings

A string containing
whitespace can be enclosed with double quotes. The token `"Hello World!"` will result in a `void*`
pointer to the null terminated characters `Hello World!\0`. To include a `"`
character in a string escape it with a backslash `"like \"this\""` to get the
string `like "this"`.

## Blocks

Blocks start with the left bracket (`{`) character. A block token ends when the next
right bracket (`}`) at the same nesting level is encountered. A right bracket can
be escaped by preceeding it with a backslash ('\').

## Load

Lines starting with  `-` load functions from a library into the
dictionary.  The characters immediately following `-` are the same as
used on the link line for the library. The next token is the return
value of the function.  The following token is the symbol to load from
the library using `dlsym(3)` declared in `<dlfcn.h>`. This returns a
`void*` pointer but does not provide any information about the signature
of the function it points to.  The tokens after that up to then next semicolon are the
are the argument types of the function.

If an argument is an ellipsis (`...`) then the function is _variadic_
and there is no need for a semicolon.

The signature of the function is also stored in
the dictionary. The symbol+signature is called a _thunk_.

## Call

Token encountered that have been loaded into the dictionary get called
with the arguments that follow.  Use `;` to have it pop from the stack
any missing arguments.  All arguments must have the same type specified
by the signature of the function.

## Variables

Lines starting with `:` define variables.

```
:i int 123
```

pushes 123 as an `int` on the stack and any future occurrence of `i` will be replaced by 123.

## Functions

Variable are just a special form of functions. 
```
:f {body}
```
causes `body` to be executed when the token `f` is encounterd.

## Stack Manipulation

Use `@<n>` to interpoate the n-th item on the stack into the
commandline, where `@` is shorthand for `@1`. Stack indexing is 1-based

This is similar to `PICK n` in forth.

Use `!<n>`
to interpolate the n-th item on the stack and remove it from the stack.
`!` is equivalent to `!1` and swaps the two top stack items.

This is similar to `ROLL <n>` in forth except subsequent parameters on a line are push on the stack.

Use `@-n` to push first n arguments on stack

Use `!-n` to remove first n arguemts on stack

## Predefined functions

The function `printf` is loaded by `lac` at startup. It has a variable number of
arguments and all arguments must be specified on the line.
See [] for how to load and call vararg functions at runtime.

## Control Flow

Lines starting with keywords have special interpretation.

```
? body
```
If the top of stack is non-zero evaluate `body`.

```
{ body }
```

executes `body` in a loop. Use `break` and `continue` to contol execution

## Variadic Functions

Variadic C functions have a fixed number of required arguments and a
variable number of optional arguments. They are loaded by specifying
the required arguments. They are called by providing both the fixed and
the variable arguments on the line on which they are called.  They don't
read arguments from the stack because they don't know how many arguments
to read.

# Unfiled

# wc

A crappy version of the unix function for counting lines, words, and characters.

```
:line int 0 -- add line to dictionary with integer value 0
:word int 0
:char int 0

fopen file.txt r ; -- FILE* since ; removes args from stack
{
	fgetc -- c FILE*
	== EOF -- (EOF == c) c FILE*
	? break -- c FILE* since ? eats tos and breaks out of block if nonzero
	incr &char -- & pushes void* pointer for thunk on the stack
	isspace c -- int c FILE*
	? incr &word ; -- c FILE* since ? eats tos then executes line if nonzero
	#isspace && incr &word
	== '\n' -- int c FILE*
	? incr &line -- c FILE*
	!! --  FILE*
}

printf "%d %d %d\n" line word char
```
## Misc

f a0 ... an -- f(a0,...,an) a1 ... an # call f, push return on top of stack
f a0 ... an ; -- f(a0,...an) # call f, remove a0 ... an, push return to top of stack

## Stack operations

Pull n-th item and push on stack. @ is the same as @0 which duplicates the top of stack.  
Similar to forth pick n.  
@n # a0 ... an -- an a0 ... an

Roll n items on the stack.  ! is the same as !1 which swaps the top two items on the stack
!n # a0 ... an -- an a1 ... an

Pop n items from stack. !! is the same as !!1
!!n # a0 ... an -- 

## Control flow

## lac

Read from FILE*

get_token into a buffer

switch on first character

-lxxx sym ret arg ... creates a thunk with key "sym"

A _thunk_ (`lac_cif`) can be called given a stack

if not special, look up "sym" in dictionary to get thunk

The thunk knows what argument types it needs.

push remaining arguments on the line right-to-left on stack using the symbol argument signature.

check missing arguments on stack using signature

call corresponding symbol

if line ends in ; pop arguments from stack

The result is pushed on the stack.

If an argument starts with '{' then everything to the matching '}' gets evaluated recursively and pushed on the stack.

FP* -> tokens to '\n' -> thunk

Load varargs function.
```
-lc int printf void* ...
```
Must be call with exact number of arguments needed on same line.

## Example

This is how a crumby version of how the unix `wc(1)` could be written in `lac`

```
:l int 0 # lines
:w int 0 # words
:c int 0 # chars
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
Once a space character is seen the unix `wc` program skips these. Define
```
:skip_space { -- FILE*
	loop {
		isspace fgets @ !? break # exit loop if character is not a space
	}
} -- FILE*
The line involving `isspace` could be replaced with
```
	isspace @ ? incr &w skip_space
```
except `skip_space` also skips newline characters. Use
:skip_space { -- FILE*
	loop {
		== '\n' fgets @ ? continue
		isspace fgets @ !? break # exit loop if character is not a space
	}
} -- FILE*
```
to avoid that.


The functions `--` and `#` get immediately executed when they are encounterd in the input stream.

## Unfiled
