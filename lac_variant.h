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

// scalar types known to ffi
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

//??? use enumerated values ???
//??? indicate if they must be free'd
// pointer types
extern ffi_type ffi_type_string;

extern ffi_type ffi_type_block; // no need!!! it is just a string

extern ffi_type ffi_type_cif;

// convert string name to ffi type
static inline const ffi_type* lac_type(const char *name)
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

// convert ffi type to string name
static inline const char* lac_name(const ffi_type* type)
{
#define X(A,B,C,D) if (type == &ffi_type_ ## B) { return # B; }
	FFI_TYPE_TABLE(X)
#undef X
	if (type == &ffi_type_variant) { return "variant"; }
	if (type == &ffi_type_string) { return "string"; }
	if (type == &ffi_type_block) { return "block"; }
	if (type == &ffi_type_cif) { return "cif"; }

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
	// size_t size; ???
} lac_variant;

//??? split out lac pointers and include their size

// return value from lac_token_parse
static inline int lac_variant_istoken(const lac_variant* pv)
{
	return pv->type == &ffi_type_string
		|| pv->type == &ffi_type_block;
}

static inline int lac_variant_true(const lac_variant v)
{
	return v.value._pointer != 0;
}
// if integer or floating point is 0 then pointer is 0
static inline int lac_variant_false(const lac_variant v)
{
	return v.value._pointer == 0;
}

// pointer address of variant value
static inline void *lac_variant_address(lac_variant * pv)
{
#define X(A,B,C,D) if (pv->type == &ffi_type_ ## B) return &pv->value._ ## B;
	FFI_TYPE_TABLE(X)
#undef X
	if (pv->type == &ffi_type_variant) {
		pv->value._pointer = &pv->value._pointer;
	}
	// all other types are pointer types
	return &pv->value._pointer;
}

static inline lac_variant* lac_variant_alloc(ffi_type* type)
{
	lac_variant* pv = malloc(sizeof(lac_variant));

	if (!pv)
		return 0;

	pv->type = type;
	pv->value._pointer = 0;

	return pv;
}

static inline void lac_variant_free(lac_variant* pv)
{
	if (lac_variant_istoken(pv)) {
		free (pv->value._pointer);
	}

	//free (pv);
}

static inline int lac_variant_cmp(const lac_variant a, const lac_variant b)
{
	int ret = a.type < b.type ? -1 : a.type > b.type ? 1 : 0;
	
	if (ret == 0) {
#define X(A,B,C,D) \
		ret = a.value._ ## B < b.value._ ## B ? -1 \
			: a.value._ ## B > b.value._ ## B ? 1 : 0;
		FFI_TYPE_TABLE(X)
#undef X
		if (lac_variant_istoken(&a)) {
			ret = strcmp(a.value._pointer, b.value._pointer);
		}
	}

	return ret;
}

// E.g. lac_variant v = lac_variant_double(1.23)
#define X(A,B,C,D) \
static inline lac_variant lac_variant_ ## B(A a) { \
	return (lac_variant){.type = &ffi_type_ ## B, .value._ ## B = a}; }
	FFI_TYPE_TABLE(X)
#undef X

// compile time type conversion
// E.g., double x = VARIANT_TO_TYPE(double, v);
#define VARIANT_TO_TYPE(T, V) *(T*)lac_variant_address(&V)

static inline void lac_variant_incr(lac_variant* pv)
{
#define X(A,B,C,D) if (pv->type == &ffi_type_ ## B) { ++pv->value._ ## B; }
	FFI_TYPE_TABLE(X)
#undef X
}
static inline void lac_variant_decr(lac_variant* pv)
{
#define X(A,B,C,D) if (pv->type == &ffi_type_ ## B) { --pv->value._ ## B; }
	FFI_TYPE_TABLE(X)
#undef X
}

// lac_variant_visit using _Generic???


// convert from string to type
static inline lac_variant lac_variant_parse(ffi_type* type, char* s)
{
	lac_variant v = { .type = type };

	if (lac_variant_istoken(&v)) {
		v.value._pointer = s;

		return v;
	}

	int ret = -1;
#define X(A,B,C,D) if (type == &ffi_type_ ## B) { \
	ret = sscanf(s, "%" D, &(v.value._ ## B)); }
	FFI_TYPE_TABLE(X)
#undef X
	if (ret < 0) {
		// false type
		v.type = &ffi_type_void;
		v.value._pointer = NULL;
	}

	return v;
}

// print formatted value to output stream
static inline int lac_variant_print(FILE * os, const lac_variant v)
{
	int ret = -1;

	if (v.type == &ffi_type_string || v.type == &ffi_type_block) {
		ret = fputs(v.value._pointer, os);
	}
	else if (v.type == &ffi_type_variant) {
		ret = lac_variant_print(os, *(const lac_variant*)v.value._pointer);
	}
	else if (v.type == &ffi_type_cif) {
		ret = lac_variant_print(os, *(const lac_variant*)v.value._pointer);
	}
	else {
#define X(A,B,C,D) if (v.type == &ffi_type_ ## B) { \
		ret = fprintf(os, "%" C, v.value._ ## B); }
		FFI_TYPE_TABLE(X)
#undef X
	}
	// check isatty(fileno(os)) ???
	if (os == stdout || os == stderr) {
		fflush(os);
	}

	return ret;		// print failed
}

// read formatted value from input stream
// set pv->type to determine conversion
static inline int lac_variant_scan(FILE * is, lac_variant * pv)
{
	if (pv->type == &ffi_type_string || pv->type == &ffi_type_block) {
		lac_variant v = lac_token_parse(is);
		if (v.type != pv->type) {
			lac_variant_free(&v);
			return EOF;
		}
		pv->value._pointer = v.value._pointer;

		return strlen(v.value._pointer); // ???
	}
#define X(A,B,C,D) if (pv->type == &ffi_type_ ## B) { \
	return fscanf(is, "%" D, &(pv->value._ ## B)); }
	FFI_TYPE_TABLE(X)
#undef X
	return EOF;		// conversion failed
}
