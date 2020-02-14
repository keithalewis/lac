// lac_variant.h -  variant type for argument stack
#pragma once
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
const ffi_type* ffi_type_lookup(const char* name);

// type for printf format string
char ffi_type_format(ffi_type* type);

// variant data type 
typedef struct {
	union {
#define X(a,b,c,d) b d;
	FFI_TYPE_TABLE(X)
#undef X
	} value;
	ffi_type* type;
} lac_variant;

// variant type for libffi
FFI_EXTERN ffi_type ffi_type_variant;

// call to initialize ffi_type_variant
void ffi_type_variant_prep(void);

// pointer address of variant value
void* lac_variant_address(lac_variant* pv);

// use compiler for type conversion
#define VARIANT_TO_TYPE(T, V) *(T*)lac_variant_address(&V)

lac_variant lac_variant_parse(ffi_type* type, const char* b, const char* e);

// parse to underlying type
#define X(A,B,C,D) B lac_parse_##D(const char* b, const char* e);
	FFI_TYPE_TABLE(X)
#undef X

// parse and put into variant
#define X(A,B,C,D) lac_variant lac_variant_parse_##D(const char* b, const char* e);
	FFI_TYPE_TABLE(X)
#undef X

// convert type to variant
#define X(A,B,C,D) lac_variant lac_variant_##D(B);
	FFI_TYPE_TABLE(X)
#undef X

// convert variant to string of at most size chars
int lac_variant_snprint(char* buf, size_t size, const lac_variant v);

