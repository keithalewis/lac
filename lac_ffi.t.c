// lac_ffi.t.c - test ffi header
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include "ensure.h"
#include "lac_ffi.h"

/*
static int incr(int i)
{
	return i+1;
}
*/

int test_lac_cif()
{
/*
  {
    ffi_cif cif;
    ffi_type *args[1];
    void *values[1];
    int i = 'a';
	int I;

    // Initialize the argument info vectors 
    args[0] = &ffi_type_sint;
    values[0] = &i;

    // Initialize the cif 
    ffi_status ret = ffi_prep_cif (&cif, FFI_DEFAULT_ABI, 1, &ffi_type_sint, args);
	if (FFI_OK == ret) {
		clock_t b = clock();
		for (i = 0; i < 10000000; ++i) {
			ffi_call (&cif, (void *) incr, &I, values);
		}
		clock_t e = clock();
		printf("ffi: %ld ms\n", (e - b)*1000/CLOCKS_PER_SEC);
		//ensure ('A' == I);
		b = clock();
		for (i = 0; i < 10000000; ++i) {
			I = incr(i);
		}
		e = clock();
		printf("C: %ld ms\n", (e - b)*1000/CLOCKS_PER_SEC);
    }
  }
*/
	{
		ffi_cif cif;
		ffi_type *args[1];
		void *values[1];
		char *s;
		ffi_arg rc;

		/* Initialize the argument info vectors */
		args[0] = &ffi_type_pointer;
		values[0] = &s;

		/* Initialize the cif */
		if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 1, &ffi_type_sint, args)
		    == FFI_OK) {
			s = "Hello World!";
			ffi_call(&cif, (void *)puts, &rc, values);
			/* rc now holds the result of the call to puts */

			/* values holds a pointer to the function's arg, so to 
			   call puts() again all we need to do is change the 
			   value of s */
			s = "This is cool!";
			ffi_call(&cif, (void *)puts, &rc, values);
		}
	}
	{
		ffi_type *args[1];
		args[0] = &ffi_type_pointer;
		lac_cif *pcif = lac_cif_alloc(&ffi_type_sint, puts, 1, args);

		void *values[1];
		char *s;

		/* Initialize the argument info vectors */
		args[0] = &ffi_type_pointer;
		values[0] = &s;

		/* Initialize the cif */
		/*
		   ffi_status ret = lac_cif_prep (pcif, &ffi_type_sint, args);
		   ensure (FFI_OK == ret);
		 */

		lac_variant result;
		s = "Hello World!";
		lac_cif_call(pcif, &result, values);
		s = "This is cool!";
		lac_cif_call(pcif, &result, values);

		lac_cif_free(pcif);
	}
	{
		ffi_type *args[2];
		args[0] = &ffi_type_pointer;

		lac_cif *pcif = lac_cif_alloc(&ffi_type_sint, printf, 1, args);
		pcif->cif.nargs = -pcif->cif.nargs;	// varargs

		// newly allocated cif
		args[1] = &ffi_type_pointer;
		lac_cif *p = lac_cif_prep_var(pcif, 1, args + 1);
		lac_cif_free(pcif);

		void *values[2];
		char *s, *fmt;

		values[0] = &fmt;
		fmt = "%s\n";

		values[1] = &s;
		s = "Hello varargs";

		lac_variant result;
		lac_cif_call(p, &result, values);
		ensure(result.value.i == strlen(s) + 1);

		lac_cif_free(p);
	}

	return 0;
}

lac_variant f(const lac_variant v)
{
	lac_variant r;
	r.type = &ffi_type_sint;
	r.value.i = 2;

	return r;
}

int test_lac_cif_variant()
{
	ffi_type_variant_prep();
	ffi_cif cif;
	ffi_type *args[1];
	void *values[1];
	lac_variant r = (lac_variant) {.type = &ffi_type_sint };
	lac_variant v = (lac_variant) {.type = &ffi_type_double,.value.d =
		    1.23 };

	/* Initialize the argument info vectors */
	args[0] = &ffi_type_variant;
	values[0] = &v;

	if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 1, &ffi_type_variant, args) ==
	    FFI_OK) {
		ffi_call(&cif, (void *)f, lac_variant_address(&r), values);
		ensure(r.type == &ffi_type_sint);
		ensure(r.value.i == 2);
	}

	return 0;
}

int test_lac_ffi()
{
	test_lac_cif();
	test_lac_cif_variant();

	return 0;
}
