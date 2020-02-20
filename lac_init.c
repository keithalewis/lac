// lac_init.c - initialize lac
#include <dlfcn.h>
#include "lac_init.h"
#include "lac_ffi.h"
#include "lac_map.h"

void map_put(const char* key, void* val)
{
	lac_map_put(key, val);
}

int RTLD_LAZY_(void)
{
	return RTLD_LAZY;
}

void put_(const char* key, void* val)
{
	lac_map_put(key, val);
}

const void* get_(const char* key)
{
	return lac_map_get(key);
}

lac_variant double_(const char*)
{
	lac_variant v;
	v.type = &ffi_type_double;
}
// load lac types
void lac_types(void)
{
}

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

	type[0] = &ffi_type_string;
	lac_map_put("puts", lac_cif_alloc(&ffi_type_sint, puts, 1, type));
}

void lac_fini(void)
{
}
