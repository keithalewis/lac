/* lac_ffi.h - load and call C functions using libffi */
//#pragma once
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <ffi.h>
#ifdef __cplusplus
}
#endif

/*
#define cbrt(X) _Generic((X), \
	long double: cbrtl, \
	default: cbrt,  \
    float: cbrtf  \
)(X)
*/

//    FFI_TYPE             type          ffi_type             union
#define FFI_TYPE_TABLE(X) \
	X(FFI_TYPE_INT,        int,         &ffi_type_sint,       i   ) \
	X(FFI_TYPE_FLOAT,      float,       &ffi_type_float,      f   ) \
	X(FFI_TYPE_DOUBLE,     double,      &ffi_type_double,     d   ) \
	X(FFI_TYPE_UINT8,      uint8_t,     &ffi_type_uint8,      u8  ) \
	X(FFI_TYPE_SINT8,      int8_t,      &ffi_type_sint8,      i8  ) \
	X(FFI_TYPE_UINT16,     uint16_t,    &ffi_type_uint16,     u16 ) \
	X(FFI_TYPE_SINT16,     int16_t,     &ffi_type_sint16,     i16 ) \
	X(FFI_TYPE_UINT32,     uint32_t,    &ffi_type_uint32,     u32 ) \
    X(FFI_TYPE_SINT32,     int32_t,     &ffi_type_sint32,     i32 ) \
	X(FFI_TYPE_UINT64,     uint64_t,    &ffi_type_uint64,     u64 ) \
	X(FFI_TYPE_SINT64,     int64_t,     &ffi_type_sint64,     i64 ) \
	X(FFI_TYPE_POINTER,    void*,       &ffi_type_pointer,    p   ) \

	// X(FFI_TYPE_VOID,       void,        &ffi_type_void,       std::monostate, v   ) \
	// X(FFI_TYPE_LONGDOUBLE, long double, &ffi_type_longdouble, std::monostate ) \
	// X(FFI_TYPE_COMPLEX
	// X(FFI_TYPE_STRUCT,     void**,      &ffi_type_pointer,    std::monostate ) \

// variant data type 
typedef struct {
	union {
#define X(a,b,c,d) b d;
	FFI_TYPE_TABLE(X)
#undef X
	} value;
	ffi_type* type;
} lac_variant;

// pointer address of variant value
void* lac_variant_address(lac_variant* pv);

lac_variant lac_variant_parse(ffi_type* type, const char* b, const char* e);

// E.g., int32_t i = lac_variant_parse_i32(b,e).i32;
#define X(A,B,C,D) lac_variant lac_variant_parse_##D(const char* b, const char* e);
	FFI_TYPE_TABLE(X)
#undef X

// value corresponding to string name of symbol
typedef struct {
	void* sym;
	ffi_cif cif;
	ffi_type* arg_types[1]; // realiy arg_types[nargs]
} lac_cif;

lac_cif* lac_cif_alloc(int nargs);
void lac_cif_free(lac_cif* cif);
size_t lac_cif_size(lac_cif* cif);

// call ffi_prep_cif using cif->cif.nargs
ffi_status lac_cif_prep(lac_cif* pcif, ffi_type* rtype, ffi_type**arg_types);
// call ffi_prep_cif_var for variadic functions where nargs is the number of variable args
// unlike ffi_prep_cif_var, arg_types only has nargs items
ffi_status lac_cif_prep_var(lac_cif* pcif, unsigned nargs, ffi_type**arg_types);
// call ffi_call
void lac_cif_call(lac_cif* pcif, ffi_arg* ret, void** args);

// argument stack
#ifndef STACK_SIZE
#define STACK_SIZE 1024
#endif

// fixed stack that grows upward
typedef struct {
	size_t sp;
	lac_variant data[STACK_SIZE + 1];
	void* addr[STACK_SIZE + 1];
} lac_stack;
// initialize with lac_stack s = {STACK_SIZE};

void lac_stack_push(lac_stack* stack, lac_variant v);
void lac_stack_pop(lac_stack* stack);
lac_variant* lac_stack_top(lac_stack* stack);
// pointer to void* stack item addresses
void* lac_stack_address(lac_stack* stack);
size_t lac_stack_size(lac_stack* stack);
// push n-th item on top of stack
// a1 a2 ... an -- an a1 ... an
lac_variant* lac_stack_pull(lac_stack* stack, size_t n);
// a1 a2 ... an -- an a1 ... an-1
lac_variant* lac_stack_roll(lac_stack* stack, size_t n);
