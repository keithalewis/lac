// lac_variant.c - variant type
#include "lac_variant.h"

static const ffi_type ffi_type_variant_union;

static const ffi_type* ffi_elements_variant[2] = {
	&ffi_type_variant_union,
	&ffi_type_pointer
};

struct struct_align_variant {
	char c;
	lac_variant x;
};
const ffi_type ffi_type_variant = {
	sizeof(lac_variant),
	offsetof(struct struct_align_variant, x),
	FFI_TYPE_VARIANT,
	(ffi_type**)ffi_elements_variant
};

// type for printf format string
char ffi_type_format(ffi_type* type)
{
#define X(A,B,C,D) if (C == type) return *#D;
	FFI_TYPE_TABLE(X)
#undef X
	return 0;
}

const ffi_type* ffi_type_lookup(const char* name)
{
	if ('d' == *name) {
		return &ffi_type_double;
	}
	if ('f' == *name) {
		return &ffi_type_float;
	}
	if ('i' == *name) {
		const char* d = strpbrk(name + 1, "8136");	
		switch (*d) {
			case '8': return &ffi_type_sint8;
			case '1': return &ffi_type_sint16;
			case '3': return &ffi_type_sint32;
			case '6': return &ffi_type_sint64;
		}
		return &ffi_type_sint;
	}
	if ('u' == *name) {
		const char* d = strpbrk(name + 1, "8136");	
		switch (*d) {
			case '8': return &ffi_type_uint8;
			case '1': return &ffi_type_uint16;
			case '3': return &ffi_type_uint32;
			case '6': return &ffi_type_uint64;
		}
		return &ffi_type_sint;
	}
	if (0 != strchr(name, '*')) {
		return &ffi_type_pointer;
	}
	if (0 == strcmp("void", name)) {
		return &ffi_type_void;
	}
	if (0 == strcmp("variant", name)) {
		return &ffi_type_variant;
	}

	return 0;
}

int lac_parse_i(const char* b, const char* e)
{
	int i;
	char* e_;

	i = (int)strtol(b, &e_, 0);
	assert (e == e_);

	return i;
}
float lac_parse_f(const char* b, const char* e)
{
	float f;
	char* e_;

	f = strtof(b, &e_);
	assert (e == e_);

	return f;
}
double lac_parse_d(const char* b, const char* e)
{
	double d;
	char* e_;

	d = strtod(b, &e_);
	assert (e == e_);

	return d;
}
uint8_t lac_parse_u8(const char* b, const char* e)
{
	uint8_t u;
	char* e_;

	u = (uint8_t)strtoul(b, &e_, 0);
	assert (e == e_);

	return u;
}
int8_t lac_parse_i8(const char* b, const char* e)
{
	int8_t i;
	char* e_;

	i = (int8_t)strtol(b, &e_, 0);
	assert (e == e_);

	return i;
}
uint16_t lac_parse_u16(const char* b, const char* e)
{
	uint16_t u;
	char* e_;

	u = (uint16_t)strtoul(b, &e_, 0);
	assert (e == e_);

	return u;
}
int16_t lac_parse_i16(const char* b, const char* e)
{
	int16_t i;
	char* e_;

	i = (int16_t)strtol(b, &e_, 0);
	assert (e == e_);

	return i;
}
uint32_t lac_parse_u32(const char* b, const char* e)
{
	uint32_t u;
	char* e_;

	u = (uint32_t)strtoul(b, &e_, 0);
	assert (e == e_);

	return u;
}
int32_t lac_parse_i32(const char* b, const char* e)
{
	int32_t i;
	char* e_;

	i = (int32_t)strtol(b, &e_, 0);
	assert (e == e_);

	return i;
}
uint64_t lac_parse_u64(const char* b, const char* e)
{
	uint64_t u;
	char* e_;

	u = (uint64_t)strtoull(b, &e_, 0);
	assert (e == e_);

	return u;
}
int64_t lac_parse_i64(const char* b, const char* e)
{
	int64_t i;
	char* e_;

	i = (int64_t)strtoll(b, &e_, 0);
	assert (e == e_);

	return i;
}
void* lac_parse_p(const char* b, const char* e)
{
	return (void*)b;
}

#define X(A,B,C,D) lac_variant \
	lac_variant_parse_##D (const char* b, const char* e) \
	{ return (lac_variant){.type = C, .value.D = lac_parse_##D(b,e)}; }
	FFI_TYPE_TABLE(X)
#undef X

inline void* lac_variant_address(lac_variant* pv)
{
#define X(a,b,c,d) if (c == pv->type) return &pv->value.d;	
	FFI_TYPE_TABLE(X)
#undef X
	return 0;
}

// Array indexed by FFI_TYPE_XXX
// static void* lac_parse_table[] = { &lac_parse_i, ... }

lac_variant lac_variant_parse(ffi_type* type, const char* b, const char* e)
{
	lac_variant v = {.value.p = 0, .type = &ffi_type_pointer};

#define X(A,B,C,D) if (C == type) { v = lac_variant_parse_##D(b,e); }
	FFI_TYPE_TABLE(X)
#undef X

	return v;
}

