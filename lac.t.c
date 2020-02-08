// lac.t.cpp
#include <math.h>
#include <dlfcn.h>
#include <stdio.h>
#include "lac_dbm.h"
#include "lac_ffi.h"
#include "lac_parse.h"

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

		lac_datum k = {"puts", 4};
		lac_datum v = {(char*)pcif, lac_cif_size(pcif)};
		lac_dbm baz = lac_dbm_open("baz");
		int ret = lac_dbm_insert(baz, k, v);
		assert (0 == ret);
		lac_datum v_ = lac_dbm_fetch(baz, k);
		lac_cif* pcif_ = (lac_cif*)v_.data;
		s = "Hello dbm";
		lac_cif_call(pcif_, &result, values);
		free (pcif_);

		lac_dbm_close(baz);
		lac_cif_free(pcif);
	}
	{
	}

	return 0;
}

int main()
{
	int test_lac_dbm(void);
	int test_lac_ffi(void);
	int test_lac_parse(void);
	int test_lac_stack(void);
	int test_lac(void);

	test_lac_dbm();
	test_lac_ffi();
	test_lac_parse();
	test_lac_stack();
	test_lac();

	return 0;
}
