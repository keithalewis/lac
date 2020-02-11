// lac.t.cpp
#include <math.h>
#include <dlfcn.h>
#include <stdio.h>
#include "ensure.h"
#include "lac_ffi.h"
#include "lac_parse.h"
#include "lac_map.h"

int test_lac(void)
{
	{
		ffi_type *args[1];
		args[0] = &ffi_type_pointer;
		lac_cif* pcif = lac_cif_alloc(&ffi_type_sint, puts, 1, args);
		void *values[1];
		char *s;

		/* Initialize the argument info vectors */
		//ffi_status ret = lac_cif_prep (pcif, &ffi_type_sint, args);
		//assert (FFI_OK == ret);

		values[0] = &s;
		s = "Hello world";
		lac_variant result;
		lac_cif_call(pcif, &result, values);
		assert (result.value.i == strlen(s) + 1); // puts writes final \0

		lac_token k = LAC_TOKEN("puts", 0);
		lac_map_put(k, pcif);
		const lac_cif* pcif_ = lac_map_get(k);
		ensure (pcif_);
		s = "Hello dbm";
		lac_cif_call(pcif_, &result, values);

		lac_cif_free(pcif);
	}

	return 0;
}

int main()
{
	int test_lac_ffi(void);
	int test_lac_parse(void);
	int test_lac_stack(void);
	int test_lac_map(void);
	int test_lac_stream();
	int test_lac(void);

	test_lac_ffi();
	test_lac_parse();
	test_lac_stack();
	test_lac_map();
	test_lac_stream();
//	test_lac();

	return 0;
}
