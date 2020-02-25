// lac_init.c - initialize lac
#include <dlfcn.h>
#include "lac_init.h"
#include "lac_ffi.h"
#include "lac_map.h"

void put_(char* key, const void* val)
{
	lac_map_put(key, val);
}

const void* get_(const char* key)
{
	return lac_map_get(key);
}

lac_variant parse_(ffi_type* type, char* s)
{
	return lac_variant_parse(type, s);
}

ffi_type* double_(void)
{
	return &ffi_type_double;
}

int print_(const lac_variant v)
{
	return lac_variant_print(stdout, v);
}

// load lac types
void lac_types(void)
{
}

#define X(A,B,C,D) lac_variant lac_type_ ## B = (lac_variant) \
	{ .type = &ffi_type_pointer, .value._pointer = &ffi_type_ ## B};
	FFI_TYPE_TABLE(X)
#undef X

#define put_ffi_type(name) \
	static lac_variant name ## _; \
	name ## _ .type = &ffi_type_pointer; \
	name ## _ .value._pointer = &ffi_type_ ## name; \
	lac_map_put(#name, &name ## _);

#define put_pointer(name) \
	static lac_variant name ## _; \
	name ## _ .type = &ffi_type_pointer; \
	name ## _ .value._pointer = name; \
	lac_map_put(#name, &name ## _);

#define put_int(name) \
	static lac_variant name ## _; \
	name ## _ .type = &ffi_type_sint; \
	name ## _ .value._sint = name; \
	lac_map_put(#name, &name ## _);

#define put_cif(name, cif_ptr) \
	static lac_variant name ## _cif; \
	name ## _cif .type = &ffi_type_cif; \
	name ## _cif .value._pointer = cif_ptr; \
	lac_map_put(#name, &name ## _cif);

void lac_init(void)
{
	ffi_type_variant_prep();

	ffi_type* type[8];

	put_pointer(stdin);
	put_pointer(stdout);
	put_pointer(stderr);

	type[0] = &ffi_type_string;
	type[1] = &ffi_type_pointer;
	lac_map_put(">", lac_cif_alloc(&ffi_type_void, put_, 2, type));

	type[0] = &ffi_type_string;
	lac_map_put("<", lac_cif_alloc(&ffi_type_pointer, get_, 1, type));

	put_int(RTLD_LAZY);
	put_int(RTLD_NOW);

	type[0] = &ffi_type_string;
	type[1] = &ffi_type_sint;
	put_cif(dlopen, lac_cif_alloc(&ffi_type_pointer, dlopen, 2, type));

	type[0] = &ffi_type_pointer;
	put_cif(dlclose, lac_cif_alloc(&ffi_type_sint, dlclose, 1, type));

	type[0] = &ffi_type_pointer;
	type[1] = &ffi_type_string;
	put_cif(dlsym, lac_cif_alloc(&ffi_type_pointer, dlsym, 2, type));

#define X(A,B,C,D) put_ffi_type(B);
	FFI_TYPE_TABLE(X)
#undef X

	type[0] = &ffi_type_pointer;
	type[1] = &ffi_type_string;
	put_cif(parse, lac_cif_alloc(&ffi_type_variant, parse_, 2, type));

	type[0] = &ffi_type_variant;
	put_cif(print, lac_cif_alloc(&ffi_type_sint, print_, 1, type));

	type[0] = &ffi_type_string;
	put_cif(puts, lac_cif_alloc(&ffi_type_sint, puts, 1, type));
}

void lac_fini(void)
{
}
