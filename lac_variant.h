// lac_variant.h -  variant type for argument stack
#pragma once
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ffi.h>

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
	X(float,          float,      "g",    "f")    \
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

// variant data type 
typedef struct {
    union {
#define X(A,B,C,D) A _ ## B;
	FFI_TYPE_TABLE(X)
#undef X
    } value;
    ffi_type *type;
} lac_variant;

// must be initialized by ffi_type_variant_prep()
extern void ffi_type_variant_prep(void);
extern ffi_type ffi_type_variant;

// pointer to null terminated characters
extern ffi_type ffi_type_string;
// pointer that has been malloc'd
extern ffi_type ffi_type_string_malloc;

// pointer to lac_cif
extern ffi_type ffi_type_cif;
extern ffi_type ffi_type_cif_malloc;

static inline lac_variant *lac_variant_alloc(void)
{
    lac_variant *pv = malloc(sizeof(lac_variant));

    if (!pv)
	return 0;

    pv->type = &ffi_type_void;
    pv->value._pointer = 0;	// zeros out union

    return pv;
}

static inline void lac_variant_free(lac_variant * pv)
{
    if (pv->type == &ffi_type_string_malloc) {
	free(pv->value._pointer);
    }
    else if (pv->type == &ffi_type_cif_malloc) {
	free(pv->value._pointer);
    }
}

// Self-contained variant + data
// If size > 0 pointer is hooked up to data
typedef struct {
	lac_variant variant;
	size_t size;
	char data[];
} lac_variant_data;

// convert string name to ffi type
static inline const ffi_type *lac_type(const char *name)
{
#define X(A,B,C,D) if (0 == strcmp(#B, name)) { return &ffi_type_ ## B; }
    FFI_TYPE_TABLE(X)
#undef X
	if (0 == strcmp("variant", name)) {
	return &ffi_type_variant;
    }
    if (0 == strcmp("string", name)) {
	return &ffi_type_string;
    }
    if (0 == strcmp("string_malloc", name)) {
	return &ffi_type_string_malloc;
    }
    if (0 == strcmp("cif", name)) {
	return &ffi_type_cif;
    }
    if (0 == strcmp("cif_malloc", name)) {
	return &ffi_type_cif_malloc;
    }

    return 0;
}

// convert ffi type to string name
static inline const char *lac_name(const ffi_type * type)
{
#define X(A,B,C,D) if (type == &ffi_type_ ## B) { return # B; }
    FFI_TYPE_TABLE(X)
#undef X
	if (type == &ffi_type_variant) {
	return "variant";
    }
    if (type == &ffi_type_string) {
	return "string";
    }
    if (type == &ffi_type_string_malloc) {
	return "string_malloc";
    }
    if (type == &ffi_type_cif) {
	return "cif";
    }
    if (type == &ffi_type_cif_malloc) {
	return "cif_malloc";
    }

    return 0;
}

// if integral or floating point is 0 then pointer is 0
static inline int lac_variant_false(const lac_variant v)
{
    return v.value._pointer == 0;
}

static inline int lac_variant_true(const lac_variant v)
{
    return v.value._pointer != 0;
}

// pointer address of variant value for ffi_call
static inline void *lac_variant_address(lac_variant * pv)
{
#define X(A,B,C,D) if (pv->type == &ffi_type_ ## B) return &pv->value._ ## B;
    FFI_TYPE_TABLE(X)
#undef X
	// all other types are pointer types
	return &pv->value._pointer;	// even lac_variant???
}

static inline int lac_variant_cmp(const lac_variant a, const lac_variant b)
{
    int ret = a.type < b.type ? -1 : a.type > b.type ? 1 : 0;

    if (ret == 0) {
	if (a.type == &ffi_type_string) {
	    return strcmp(a.value._pointer, b.value._pointer);
	}
	if (a.type == &ffi_type_string_malloc) {
	    return strcmp(a.value._pointer, b.value._pointer);
	}
#define X(A,B,C,D) \
		if (a.type == &ffi_type_ ## B) return \
			a.value._ ## B < b.value._ ## B \
			? -1 : a.value._ ## B > b.value._ ## B \
			? 1 : 0;
	FFI_TYPE_TABLE(X)
#undef X
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
// convert from string to type
static inline lac_variant lac_variant_parse(ffi_type * type, char *s)
{
    lac_variant v = {.type = type };

    if (type == &ffi_type_variant) {
	v.type = &ffi_type_string;	// malloc???
	v.value._pointer = s;
    }
    else if (type == &ffi_type_string) {
	v.value._pointer = s;
    }
    else if (type == &ffi_type_string_malloc) {
	size_t n = strlen(s);
	v.value._pointer = malloc(n + 1);
	if (!v.value._pointer) {
	    // return null pointer, just like malloc
	    v.type = &ffi_type_pointer;
	    v.value._pointer = 0;

	    return v;
	}
	strcpy(v.value._pointer, s);
    }
    else {
	int ret = -1;
#define X(A,B,C,D) if (type == &ffi_type_ ## B) { \
		ret = sscanf(s, "%" D, &(v.value._ ## B)); }
	FFI_TYPE_TABLE(X)
#undef X
	    // pointer scan is questionable!!!
	    if (ret < 0) {
	    // false type
	    v.type = &ffi_type_void;
	    v.value._pointer = NULL;
	}
    }

    return v;
}

// print formatted value to output stream
static inline int lac_variant_print(FILE * os, const lac_variant * pv)
{
    int ret = -1;

    if (pv->type == &ffi_type_string) {
	ret = fputs(pv->value._pointer, os);
    }
    else if (pv->type == &ffi_type_string_malloc) {
	ret = fputs(pv->value._pointer, os);
    }
    else if (pv->type == &ffi_type_variant) {
	ret = lac_variant_print(os, (const lac_variant *)pv->value._pointer);
    }
    else {
	if (0) {;
	}			// force first match in FFI_TYPE_TABLE
#define X(A,B,C,D) \
		else if (pv->type == &ffi_type_ ## B) { \
			ret = fprintf(os, "%" C, (A)pv->value._ ## B); \
		}
	FFI_TYPE_TABLE(X)
#undef X
    }

    // check isatty(fileno(os)) ???
    if (os == stdout || os == stderr) {
	fflush(os);
    }

    return ret;
}

/* use visit ???
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
*/

// lac_variant_visit using _Generic???
