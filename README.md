# Load And Call

Load and call C functions at runtime.

This little language makes it possible to dynamically load C
functions from shared libraries and call them at runtime.

Just specify the libray, the symbol, it's return value and
the argument values.

```
-lm cos double double # load cosine function for libm.so
cos 0 # call cos(0) and push on stack
printf %g @ # call printf("%g", cos(0))
```
Use `@<n>` to use the n-th item on the stack, where `@` is shorthand for `@1`.
You can also use `!<n>` to use the n-th item and remove it from the stack.
