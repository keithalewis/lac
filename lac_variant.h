// lac_variant.h -  variant type for argument stack
#pragma once
#include <errno.h>
#include <inttypes.h>
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
//    type            ffi_type_x  print   scan
#define FFI_TYPE_TABLE(X) \
	X(char,           schar,      "c"   , "c") \
	X(unsigned char,  uchar,      "c"   , "c") \
	X(int,            sint,       "i"   , "i") \
	X(unsigned int,   uint,       "u"   , "u") \
	X(short,          sshort,     "hi"  , "hi") \
	X(unsigned short, ushort,     "hu"  , "hu") \
	X(long,           slong,      "li"  , "li") \
	X(unsigned long,  ulong,      "lu"  , "lu") \
	X(float,          float,      "f"   , "f") \
	X(double,         double,     "g"   , "lf") \
	X(uint8_t,        uint8,      PRIu8 , SCNu8) \
	X(int8_t,         sint8,      PRIi8 , SCNi8) \
	X(uint16_t,       uint16,     PRIu16, SCNu16) \
	X(int16_t,        sint16,     PRIi16, SCNi16) \
	X(uint32_t,       uint32,     PRIu32, SCNu32) \
    X(int32_t,        sint32,     PRIi32, SCNi32) \
	X(uint64_t,       uint64,     PRIu64, SCNu64) \
	X(int64_t,        sint64,     PRIi64, SCNi64) \
	X(void*,          pointer,   "p"    , "p") \

// X(FFI_TYPE_VOID,       void,        &ffi_type_void,
// X(FFI_TYPE_LONGDOUBLE, long double, &ffi_type_longdouble,
// X(FFI_TYPE_COMPLEX
// X(FFI_TYPE_STRUCT,     void**,      &ffi_type_pointer, 

// convert string name to ffi type
static inline const ffi_type *ffi_type_lookup(const char *name)
{
#define X(A,B,C,D) if (0 == strcmp(#B, name)) { return &ffi_type_ ## B; }
	FFI_TYPE_TABLE(X)
#undef X
    return 0;
}

// type for printf format string
static inline char ffi_type_format(ffi_type * type)
{
#define X(A,B,C,D) if (type == &ffi_type_ ## B) return (#D)[0];	// return #D ???
	FFI_TYPE_TABLE(X)
#undef X
    return 0;
}

// variant data type 
typedef struct {
	union {
#define X(A,B,C,D) A _ ## B;
		FFI_TYPE_TABLE(X)
#undef X
	} value;
	ffi_type *type;
} lac_variant;

// pointer address of variant value
static inline void *lac_variant_address(lac_variant * pv)
{
#define X(A,B,C,D) if (pv->type == &ffi_type_ ## B) return &pv->value._ ## B;
	FFI_TYPE_TABLE(X)
#undef X
	    return 0;
}

// compile time type conversion
#define VARIANT_TO_TYPE(T, V) *(T*)lac_variant_address(&V)

static inline int lac_variant_print(FILE * os, const lac_variant v)
{
#define X(A,B,C,D) if (v.type == &ffi_type_ ## B) { \
	return fprintf(os, "%" C, v.value._ ## B); }

	FFI_TYPE_TABLE(X)
#undef X
	return -1;		// print failed
}

// set pv->type to determine conversion
static inline int lac_variant_scan(FILE * is, lac_variant * pv)
{
#define X(A,B,C,D) if (pv->type == &ffi_type_ ## B) { \
	return fscanf(is, "%" D, &(pv->value._ ## B)); }

	FFI_TYPE_TABLE(X)
#undef X
	return EOF;		// conversion failed
}
