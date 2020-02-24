# [libffi](https://sourceware.org/libffi/)

Anthony Green originally developed `libffi`. It makes it possible to
call C functions on a wide variety of processor architectures given a
pointer to a function and the addresses of the function arguments.

_Platform_ refers to the chip architecture and operating system.

The C InterFace, of a function consists of its return type and
required argument types. The `ffi_cif` struct contains this information
and additional platform specific information.

Types are described by a `ffi_type` struct. It consists of the size of the
type, its platform specific alignment, a type identifier, and an array
of pointers to other `ffi_type`s if it is a struct or union.

The `ffi_cif` must be prepared to set platform specific information.
The function 
```
ffi_status ffi_prep_cif(ffi_cif *cif, ffi_abi abi,
unsigned int nargs, ffi_type *rtype, ffi_type **argtypes)
```
populates `cif` with the required information to call a C function.

Use `abi = FFI_DEFAULT_ABI` for standard C functions.

A C function can be called with a pointer to its symbol and the addresses
of its required arguments. A pointer to memory with sufficient space to
accomodate the return value must also be provided.

The function 
```
void ffi_call(ffi_cif *cif, void *fn, void *rvalue, void **avalues)
```
uses the prepared `ffi_cif` to arrange `avalues` on the call
stack, call `fn`, and deposit the result into the memory pointed to by
`rvalue`. It knows nothing about memory allocation and requires the user
to make all necessary arrangements to ensure the memory being pointed
to exists.

There is a minium size for return values. They are widened to the
register size of the platform.  The type `ffi_arg` is large enough to
hold them. There is no minimum size for arguments.

The cif for variadic functions must be prepared with both the the
fixed and variable arguments.
```
ffi_status ffi_prep_cif_var(ffi_cif *cif, ffi_abi abi,
unsigned int nfixedargs, unsigned int ntotalargs, ffi_type *rtype, ffi_type **argtypes)
```

It can then be used in `ffi_call` with `ntotalargs` in `avalues` pointing
at all of the fixed and variable argument addresses.

A union is a `ffi_type` of type struct with one element that has the
largest type and alignment of all the types in the union.
[https://stackoverflow.com/questions/40354500/how-do-i-create-an-ffi-type-that-represents-a-union]








A union is a struct with one element having maximum size and alignment.
