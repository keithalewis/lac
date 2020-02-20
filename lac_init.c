// lac_init.c - initialize lac
#include "lac_init.h"
#include "lac_ffi.h"
#include "lac_map.h"

void map_put(const char* key, void* val)
{
	lac_map_put(key, val);
}

void lac_init(void)
{
	ffi_type_variant_prep();

	ffi_type* type[1];
	type[0] = &ffi_type_string;
	lac_map_put("puts", lac_cif_alloc(&ffi_type_sint, puts, 1, type));
}
