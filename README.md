# Load And Call

Load and call C functions at runtime.

This little language makes it possible to dynamically load C
functions from shared libraries at runtime and call them.

```
-lm cos double double # load double cos(double) from libm.so/.dll
cos 0                 # call cos(0.) and push the result on the stack
```

The symbol `cos` will be added to the _dictionary_ and lines starting with `cos`
will expect to find a `double` argument either on the line or on the _stack_
and call the C function. The argument is consumed and the result is pushed
on the stack.

Another way to call `cos(0.)` is

```
double 0
cos
```

The first line pushes 0 as a double on to the stack. When `cos` is evaluated without
specifying an argument on the same line it will look for the arguments in needs
on the stack.

The possible stack types are `int`, `float`, `double`, `uint8_t`, `int8_t`, `uint16_t`, `int16_t`,
`uint32_t`, `int32_t`, `uint64_t`, `int64_t`, and `void*`.

`lac` is line oriented. Use backslash ('\') at the end of a line to continue parsing to the next line.  

Lines starting with  `-` load functions that can be called.

Lines starting with functions that have been loaded get called with the
arguments that follow.  Any missing arguments must be supplied on the
stack. All arguments must have the same type specified by the signature of
the function.

Lines starting with `:` define variables.

```
:i int 123
```

pushes 123, as an `int`, on the stack and any future occurrence of `i` will be replaced by 123.


```
:var line ...
```

evaluates `line ...` and assignes the resulting top of stack to `var`.

To delay evaluation enclose lines with brackets: `{line}`. Future occurences of `var`
will be substitued by `line ...` and evaluated in the current context.

printf %g\n @         # call printf("%g\n", cos(0.))
```

Use `@<n>` to interpoate the n-th item on the stack into the
commandline, where `@` is shorthand for `@1`.  Use `!<n>`
to interpolate the n-th item on the stack and remove it from the stack.

??? This is similar to `ROLL <n>` in forth except subsequent parameters are push on the stack.

The function `printf` is loaded by `lac` at startup. It has a variable number of
arguments and all arguments must be specified on the line.
See [] for how to load and call vararg functions at runtime.


A line starting with `-` results in a function being loaded that will 

If a string argument includes spaces, use double quotes 

Variables can be defined by

```
:var expr
```


## Variadic Functions

Variadic C functions have a fixed number of required arguments and a variable number
of optional arguments. They are loaded by specifying the required arguments. They
are called by providing the variable arguments on the line on which they are called.

# Unfiled

lac

line: -...
      :...
	  else

-l<lib> <sym> <ret> <arg>...

lacod

sym val... -> push ret on stack

:name line


