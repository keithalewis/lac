// lac_ffi.c
#include "ensure.h"
#include "lac_ffi.h"

ffi_type* ffi_type_lookup(const char* name)
{
	if ('d' == *name) {
		return &ffi_type_double;
	}
	if ('f' == *name) {
		return &ffi_type_float;
	}
	if ('v' == *name) {
		return 0 == strchr(name + 1, '*') ? &ffi_type_void : &ffi_type_pointer;
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

#define X(A,B,C,D) lac_variant lac_variant_parse_##D(const char* b, const char* e) \
	{ lac_variant v; v.type = C; v.value.D = lac_parse_##D(b,e); return v; }
	FFI_TYPE_TABLE(X)
#undef X

inline void* lac_variant_address(lac_variant* pv)
{
#define X(a,b,c,d) if (c == pv->type) return &pv->value.d;	
	FFI_TYPE_TABLE(X)
#undef X
	return 0;
}

// int lac_parse_i(const char* b, const char* e)
// double lac_parse_d(const char* b, const char* e)
// ...
// 
// Array indexed by FFI_TYPE_XXX
// static void* lac_parse_table[] = { &lac_parse_i, ... }

lac_variant lac_variant_parse(ffi_type* type, const char* b, const char* e)
{
	lac_variant v;

#define X(A,B,C,D) if (C == type) { v = lac_variant_parse_##D(b,e); }
	FFI_TYPE_TABLE(X)
#undef X

	return v;
}

size_t lac_cif_size(lac_cif* pcif)
{
	return sizeof(lac_cif) + pcif->cif.nargs*sizeof(void*);
}

// Allocate and set nargs
lac_cif* lac_cif_alloc(ffi_type* rtype, void* sym,
	unsigned nargs, ffi_type **arg_types)
{
	lac_cif* p = malloc(sizeof(lac_cif) + nargs*sizeof(void*));
	ensure (0 != p);

	p->cif.rtype = rtype;
	p->sym = sym;
	p->cif.nargs = nargs;
	p->cif.arg_types = &p->arg_types[0];
	memcpy(p->arg_types, arg_types, nargs*sizeof(ffi_type*));

	ffi_status ret;
	ret = ffi_prep_cif(&p->cif, FFI_DEFAULT_ABI, nargs, rtype, arg_types);
	ensure (FFI_OK == ret);

	return p;
}
// preserve existing arg_types
lac_cif* lac_cif_realloc(lac_cif* p, unsigned n)
{
	p = realloc(p, sizeof(lac_cif) + n*sizeof(void*));
	ensure (0 != p);

	p->cif.nargs = n;
	p->cif.arg_types = &p->arg_types[0];

	return p;
}

void lac_cif_free(lac_cif* p)
{
	free(p);
}

ffi_status
lac_cif_prep(lac_cif* pcif, ffi_type* rtype, ffi_type**arg_types)
{
	pcif->cif.arg_types = &pcif->arg_types[0];
	memcpy(pcif->arg_types, arg_types, pcif->cif.nargs*sizeof(void*));

	return ffi_prep_cif(&pcif->cif, FFI_DEFAULT_ABI,
	                    pcif->cif.nargs, rtype, pcif->arg_types);
}

// prepare additional args for vararg functions
lac_cif* lac_cif_prep_var(lac_cif* p, unsigned nargs, ffi_type** arg_types)
{
	unsigned nfix = p->cif.nargs; // number of fixed args
	lac_cif* p_ = malloc(sizeof(lac_cif) + (nfix + nargs)*sizeof(void*));
	ensure (0 != p_);

	p_->cif.rtype = p->cif.rtype;
	p_->sym = p->sym;
	p_->cif.nargs = p->cif.nargs + nargs;
	p_->cif.arg_types = &p->arg_types[0];
	memcpy(p_->cif.arg_types, p->cif.arg_types, nfix*sizeof(ffi_type*));
	memcpy(p_->cif.arg_types + nfix, arg_types, nargs*sizeof(void*));

	ffi_status ret = ffi_prep_cif_var(&p_->cif, FFI_DEFAULT_ABI, nfix,
                   	 nfix + nargs, p_->cif.rtype, p_->cif.arg_types);
	ensure (FFI_OK == ret);

	return p_;
}

void lac_cif_call(lac_cif* pcif, lac_variant* result, void** args)
{
	result->type = pcif->cif.rtype;

	ffi_call(&pcif->cif, pcif->sym, lac_variant_address(result), args);
}
