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
		assert (FFI_OK == ret);

		values[0] = &s;
		s = "Hello world";
		lac_cif_call(pcif, &rc, values);

		lac_datum k = {"puts", 4};
		lac_datum v = {(char*)pcif, lac_cif_size(pcif)};
		lac_dbm baz = lac_dbm_open("baz");
		ret = lac_dbm_insert(baz, k, v);
		assert (0 == ret);
		lac_datum v_ = lac_dbm_fetch(baz, k);
		lac_cif* pcif_ = (lac_cif*)v_.data;
		s = "Hello dbm";
		lac_cif_call(pcif_, &rc, values);
		free (pcif_);

		lac_dbm_close(baz);

		/* Initialize the cif */
		lac_cif_free(pcif);

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
