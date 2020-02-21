// lac_init.c - initialize lac
#include <dlfcn.h>
#include "lac_init.h"
#include "lac_ffi.h"
#include "lac_map.h"

int RTLD_LAZY_(void)
{
	return RTLD_LAZY;
}

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

void lac_init(void)
{
	ffi_type_variant_prep();

	ffi_type* type[8];

	type[0] = &ffi_type_string;
	type[1] = &ffi_type_pointer;
	lac_map_put(">", lac_cif_alloc(&ffi_type_void, put_, 2, type));

	type[0] = &ffi_type_string;
	lac_map_put("<", lac_cif_alloc(&ffi_type_pointer, get_, 1, type));

	lac_map_put("RTLD_LAZY", lac_cif_alloc(&ffi_type_sint, RTLD_LAZY_, 0, NULL));
	//lac_map_put("RTLD_NOW", 

	type[0] = &ffi_type_string;
	type[1] = &ffi_type_sint;
	lac_map_put("dlopen", lac_cif_alloc(&ffi_type_pointer, dlopen, 2, type));

	type[0] = &ffi_type_pointer;
	lac_map_put("dlclose", lac_cif_alloc(&ffi_type_sint, dlclose, 1, type));

	type[0] = &ffi_type_pointer;
	type[1] = &ffi_type_string;
	lac_map_put("dlsym", lac_cif_alloc(&ffi_type_pointer, dlsym, 2, type));

#define X(A,B,C,D) lac_map_put(#B, &lac_type_ ## B);
	FFI_TYPE_TABLE(X)
#undef X

	type[0] = &ffi_type_pointer;
	type[1] = &ffi_type_string;
	lac_map_put("parse", lac_cif_alloc(&ffi_type_variant, parse_, 2, type));

	type[0] = &ffi_type_variant;
	lac_map_put("print", lac_cif_alloc(&ffi_type_sint, print_, 1, type));
}

void lac_fini(void)
{
}
