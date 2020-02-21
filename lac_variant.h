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
#include "ensure.h"

//    type            ffi_type    print   scan
#define FFI_TYPE_TABLE(X)                         \
	X(char,           schar,      "c",    "c")    \
	X(unsigned char,  uchar,      "c",    "c")    \
	X(int,            sint,       "i",    "i")    \
	X(unsigned int,   uint,       "u",    "u")    \
	X(short,          sshort,     "hi",   "hi")   \
	X(unsigned short, ushort,     "hu",   "hu")   \
	X(long,           slong,      "li",   "li")   \
	X(unsigned long,  ulong,      "lu",   "lu")   \
	X(float,          float,      "f",    "f")    \
	X(double,         double,     "g",    "lf")   \
	X(uint8_t,        uint8,      PRIu8,  SCNu8)  \
	X(int8_t,         sint8,      PRIi8,  SCNi8)  \
	X(uint16_t,       uint16,     PRIu16, SCNu16) \
	X(int16_t,        sint16,     PRIi16, SCNi16) \
	X(uint32_t,       uint32,     PRIu32, SCNu32) \
    X(int32_t,        sint32,     PRIi32, SCNi32) \
	X(uint64_t,       uint64,     PRIu64, SCNu64) \
	X(int64_t,        sint64,     PRIi64, SCNi64) \
	X(void*,          pointer,    "p",    "p")    \

// X(FFI_TYPE_VOID,       void,        &ffi_type_void,
// X(FFI_TYPE_LONGDOUBLE, long double, &ffi_type_longdouble,
// X(FFI_TYPE_COMPLEX
// X(FFI_TYPE_STRUCT,     void**,      &ffi_type_pointer, 

// must be initialized by ffi_type_variant_prep()
extern ffi_type ffi_type_variant;

// pointer types
extern ffi_type ffi_type_string;

extern ffi_type ffi_type_block;

extern ffi_type ffi_type_cif;

// convert string name to ffi type
static inline const ffi_type *ffi_type_lookup(const char *name)
{
#define X(A,B,C,D) if (0 == strcmp(#B, name)) { return &ffi_type_ ## B; }
	FFI_TYPE_TABLE(X)
#undef X
	if (0 == strcmp("variant", name)) { return &ffi_type_variant; }
	if (0 == strcmp("string", name)) { return &ffi_type_string; }
	if (0 == strcmp("block", name)) { return &ffi_type_block; }
	if (0 == strcmp("cif", name)) { return &ffi_type_cif; }

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
	if (pv->type == &ffi_type_string || pv->type == &ffi_type_block) {
		return &pv->value._pointer;
	}
#define X(A,B,C,D) if (pv->type == &ffi_type_ ## B) return &pv->value._ ## B;
	FFI_TYPE_TABLE(X)
#undef X

	return 0;
}

#define X(A,B,C,D) \
static inline lac_variant lac_variant_ ## B(A a) { \
	return (lac_variant){.type = &ffi_type_ ## B, .value._ ## B = a}; }
	FFI_TYPE_TABLE(X)
#undef X

static inline void lac_variant_free(lac_variant* pv)
{
	if (pv->type == &ffi_type_string || pv->type == &ffi_type_block) {
		free (pv->value._pointer);
	}

	pv->type = &ffi_type_void;
}

// compile time type conversion
#define VARIANT_TO_TYPE(T, V) *(T*)lac_variant_address(&V)

static inline int lac_variant_equal(const lac_variant a, const lac_variant b)
{
	if (a.type != b.type) {
		return 0;
	}
#define X(A,B,C,D) if (a.value._ ## B == a.value._ ## B) { return 1; }
	FFI_TYPE_TABLE(X)
#undef X
	if (a.type == &ffi_type_string || a.type == &ffi_type_block) {
		return 0 == strcmp(a.value._pointer, b.value._pointer);
	}

	return 0;
}

static inline void lac_variant_incr(lac_variant* pv)
{
	if (pv->type == &ffi_type_string || pv->type == &ffi_type_block) {
		return;
	}
#define X(A,B,C,D) if (pv->type == &ffi_type_ ## B) { ++pv->value._ ## B; }
	FFI_TYPE_TABLE(X)
#undef X
}
static inline void lac_variant_decr(lac_variant* pv)
{
	if (pv->type == &ffi_type_string || pv->type == &ffi_type_block) {
		return;
	}
#define X(A,B,C,D) if (pv->type == &ffi_type_ ## B) { --pv->value._ ## B; }
	FFI_TYPE_TABLE(X)
#undef X
}

// tokens are separated by white space (isspace)
// tokens starting with double quote ('"') end at the next quote
//   use '\' to escape '"`
// tokens starting with left curly bracket ('{') end the next
//   right curly bracket ('}') at the same nesting level.
//   use '\' to escape '}`
// returned pointer must be freed
lac_variant lac_parse_token(FILE *);

// convert from string to type
static inline lac_variant lac_variant_parse(ffi_type* type, char* s)
{
	lac_variant v = { .type = &ffi_type_void };

	// null strings???
	if (type == &ffi_type_string || type == &ffi_type_block) {
		v.type = type;
		v.value._pointer = s;

		return v;
	}

	int ret;
#define X(A,B,C,D) if (type == &ffi_type_ ## B) { \
	ret = sscanf(s, "%" D, &(v.value._ ## B)); }
	FFI_TYPE_TABLE(X)
#undef X
	ensure (ret > 0);

	v.type = type;

	return v;
}

// print formatted value to output stream
static inline int lac_variant_print(FILE * os, const lac_variant v)
{
	if (v.type == &ffi_type_string || v.type == &ffi_type_block) {
		return fputs(v.value._pointer, os);
	}
#define X(A,B,C,D) if (v.type == &ffi_type_ ## B) { \
	return fprintf(os, "%" C, v.value._ ## B); }
	FFI_TYPE_TABLE(X)
#undef X
	return -1;		// print failed
}

// read formatted value from input stream
// set pv->type to determine conversion
static inline int lac_variant_scan(FILE * is, lac_variant * pv)
{
	if (pv->type == &ffi_type_string || pv->type == &ffi_type_block) {
		lac_variant v = lac_parse_token(is);
		if (v.type != pv->type) {
			lac_variant_free(&v);
			return EOF;
		}
		pv->value._pointer = v.value._pointer;

		return strlen(v.value._pointer);
	}
#define X(A,B,C,D) if (pv->type == &ffi_type_ ## B) { \
	return fscanf(is, "%" D, &(pv->value._ ## B)); }
	FFI_TYPE_TABLE(X)
#undef X
	return EOF;		// conversion failed
}
