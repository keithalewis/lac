// lac_variant.h -  variant type for argument stack
#pragma once
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <ffi.h>
#ifdef __cplusplus
}
#endif

//    FFI_TYPE             type          ffi_type             union
#define FFI_TYPE_TABLE(X) \
	X(FFI_TYPE_INT,        int,         &ffi_type_sint,       i   ) \
	X(FFI_TYPE_FLOAT,      float,       &ffi_type_float,      f   ) \
	X(FFI_TYPE_DOUBLE,     double,      &ffi_type_double,     g   ) \
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
static inline const ffi_type* ffi_type_lookup(const char* name)
{
#define X(A,B,C,D) if (0 == strcmp(#B, name)) { return C; }
	FFI_TYPE_TABLE(X)
#undef X
	return 0;
}

// type for printf format string
static inline char ffi_type_format(ffi_type* type)
{
#define X(A,B,C,D) if (C == type) return (#D)[0];
	FFI_TYPE_TABLE(X)
#undef X
	return 0;
}

// variant data type 
typedef struct {
	union {
#define X(A,B,C,D) B D;
	FFI_TYPE_TABLE(X)
#undef X
	} value;
	ffi_type* type;
} lac_variant;

// pointer address of variant value
static inline void* lac_variant_address(lac_variant* pv)
{
#define X(a,b,c,d) if (c == pv->type) return &pv->value.d;	
	FFI_TYPE_TABLE(X)
#undef X
	return 0;
}

// compile time type conversion
#define VARIANT_TO_TYPE(T, V) *(T*)lac_variant_address(&V)

static inline int lac_variant_print(FILE* os, const lac_variant v)
{
	char fmt[] = "%?";
#define X(A,B,C,D) if (v.type == C) { \
	fmt[1] = ffi_type_format(v.type); \
	return fprintf(os, fmt, v.value.D); }

	FFI_TYPE_TABLE(X)
#undef X
	return -1; // print failed
}

// set pv->type to determine conversion
static inline int lac_variant_scan(FILE* is, lac_variant* pv)
{
	char fmt[] = "%?";
#define X(A,B,C,D) if (pv->type == C) { \
	fmt[1] = ffi_type_format(pv->type); \
	return fscanf(is, fmt, &pv->value.D); }

	FFI_TYPE_TABLE(X)
#undef X
	return EOF; // conversion failed
}

/*
// variant type for libffi
FFI_EXTERN ffi_type ffi_type_variant;

// call to initialize ffi_type_variant
void ffi_type_variant_prep(void);
*/



