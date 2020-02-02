# Load And Call

Load and call C functions at runtime.

This little language makes it possible to dynamically load C
functions from shared libraries at runtime and call them.

```
-lm cos double double # load double cos(double) from libm.so/.dll
cos 0                 # call cos(0.) and push the result on the stack
```

The symbol `cos` will be added to the _dictionary_ and lines starting with `cos`
will expect to find a `double` argument either on the rest of the line or on the _stack_
and call the C function. The argument is consumed and the result is pushed
on the stack.

Another way to call `cos(0.)` is

```
double 0
cos
```

The first line pushes 0 as a double on to the stack.
When `cos` is evaluated without specifying an argument on the same line
it will look for the arguments it needs on the stack.

The possible stack types are `int`, `float`, `double`, `uint8_t`, `int8_t`, `uint16_t`, `int16_t`,
`uint32_t`, `int32_t`, `uint64_t`, `int64_t`, and `void*`.

`lac` is line oriented. Use backslash ('\\') at the end of a line to continue parsing to the next line.  
Tokens are separated by whitespace using `isspace` from `<ctypes.h>`. A string containing
whitespace can be enclosed with double quotes. The token `"Hello World!"` will result in a `void*`
pointer to the null terminated characters `Hello World!\0` on the stack. To include a `"`
character in a string escape it with a backslash `"like \"this\""` to get the
string `like "this"`.


## Load

Lines starting with  `-` load functions from a library into the dictionary.
The characters immediately following `-` are the same as used on
the link line for the library. The next token is the
symbol to load from the library using `dlsym(3)` declared in
`<dlfcn.h>`. This returns a `void*` pointer but does not
provide any information about the signature of the function
it points to.

The token following the symbol name is the return type of the function.
The tokens after that are the argument types of the function.
The signature of the function is also stored in the dictionary.



## Call

Lines starting with functions that have been loaded into the dictionary
get called with the arguments that follow.  Any missing arguments must
be supplied on the stack. All arguments must have the same type specified
by the signature of the function.


## Variables

Lines starting with `:` define variables.

```
:i int 123
```

pushes 123 as an `int` on the stack and any future occurrence of `i` will be replaced by 123.
Values are pushed on the stack from right to left.

```
:var line ...
```

evaluates `line ...` and assignes the resulting top of stack to `var`.

To delay evaluation enclose `line ...` with brackets: `{line ...}`. Future occurences of `var`
will be substitued by `line ...` and evaluated using the current dictionary and stack.

Brackets can be nested, `{a {b} c}` pushes the string `a {b} c` onto the stack. As with strings,
right brackets can be escaped if immediately preceeded by a backslash.

## Stack Manipulation

Use `@<n>` to interpoate the n-th item on the stack into the
commandline, where `@` is shorthand for `@1`.  Use `!<n>`
to interpolate the n-th item on the stack and remove it from the stack.

??? This is similar to `ROLL <n>` in forth except subsequent parameters on a line are push on the stack.

The function `printf` is loaded by `lac` at startup. It has a variable number of
arguments and all arguments must be specified on the line.
See [] for how to load and call vararg functions at runtime.

## Control Flows

Lines starting with keywords have special interpretation.

```
if expr {
	body
}
```

executes `body` if `expr` evaluates to a non-zero value

```
while expr {
	body
}
```

executes `body` as long as `expr` evaluates to a non-zero value.

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
line: int 0
word: int 0
char: int 0

fopen file.txt r -- FILE*
c: {fgetc @}

while != EOF c {
	incr &char
	if isspace c {
		incr &word
	}
	if == \n c {
		incr &line
	}
}
printf "%d %d %d\n" line word char
pop --
```

## Misc

f a1 ... an # call f, push return to top of stack
f a1 ... an ; # call f, remove a1 ... an, push return to top of stack

Stack operations

@n # a1 ... an -- an a1 ... an
!n # a1 ... an -- an a1 ... an-1 similar to roll n
!-n # a1 ... an -- an # clear to n

Control flow

:lable # with no arguments stores current state
jump lable # got to label are reexecute
