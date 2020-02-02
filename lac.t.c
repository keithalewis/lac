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
		lac_cif* pcif = lac_cif_alloc(1);
		pcif->sym = puts;
		ffi_type *args[1];
		void *values[1];
		char *s;
		ffi_arg rc;

		/* Initialize the argument info vectors */
		args[0] = &ffi_type_pointer;
		ffi_status ret = lac_cif_prep (pcif, &ffi_type_sint, args);


		values[0] = &s;

		/* Initialize the cif */
		assert (FFI_OK == ret);

	}

	return 0;
}

int main()
{
	int test_lac_dbm(void);
	int test_lac_ffi(void);
	int test_lac_parse(void);
	int test_lac(void);

	test_lac_dbm();
	test_lac_ffi();
	test_lac_parse();
	test_lac();

	return 0;
}
