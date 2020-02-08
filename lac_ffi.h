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

	// X(FFI_TYPE_VOID,       void,        &ffi_type_void,
	// X(FFI_TYPE_LONGDOUBLE, long double, &ffi_type_longdouble,
	// X(FFI_TYPE_COMPLEX
	// X(FFI_TYPE_STRUCT,     void**,      &ffi_type_pointer, 

// convert string name to ffi type
ffi_type* ffi_type_lookup(const char* name);

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

#define X(A,B,C,D) B lac_parse_##D(const char* b, const char* e);
	FFI_TYPE_TABLE(X)
#undef X

// E.g., int32_t i = lac_variant_parse_i32(b,e).i32;
#define X(A,B,C,D) lac_variant lac_variant_parse_##D(const char* b, const char* e);
	FFI_TYPE_TABLE(X)
#undef X

// value corresponding to string name of symbol
typedef struct {
	void* sym;
	ffi_cif cif;
	ffi_type* arg_types[]; // flexible length array
} lac_cif;

// allocate n args and set cif.arg_types to arg_types
lac_cif* lac_cif_alloc(ffi_type* rtype, void* sym,
	unsigned nargs, ffi_type **arg_types);
// reallocate n args and set cif.arg_types to arg_types
// preserve existing arg types
lac_cif* lac_cif_realloc(lac_cif* pcif, unsigned nargs);
void lac_cif_free(lac_cif* pcif);

size_t lac_cif_size(lac_cif* pcif);

// call ffi_prep_cif using cif->cif.nargs
ffi_status lac_cif_prep(lac_cif* pcif, ffi_type* rtype, ffi_type** arg_types);

// call ffi_prep_cif_var for variadic functions
// where nargs is the number of variable args
// unlike ffi_prep_cif_var, arg_types only has nargs items
lac_cif* lac_cif_prep_var(lac_cif* pcif, unsigned nargs, ffi_type** arg_types);

// call ffi_call and store result
void lac_cif_call(lac_cif* pcif, lac_variant* result, void** args);
