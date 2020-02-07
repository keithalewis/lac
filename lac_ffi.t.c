// lac_ffi.t.c - test ffi header
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include "ensure.h"
#include "lac_ffi.h"

#define VIEW(s) s, s + sizeof(s) - 1

int
test_lac_variant_parse ()
{
  {
    lac_variant v = lac_variant_parse (&ffi_type_sint, VIEW ("123"));
    ensure (&ffi_type_sint == v.type);
    ensure (123 == v.value.i);
    ensure (lac_variant_address (&v) == &v.value.i);
  }
  {
    lac_variant v = lac_variant_parse (&ffi_type_sint, VIEW ("123 ") - 1);
    ensure (&ffi_type_sint == v.type);
    ensure (123 == v.value.i);
  }
  {
    lac_variant v = lac_variant_parse (&ffi_type_sint, VIEW ("123x") - 1);
    ensure (&ffi_type_sint == v.type);
    ensure (123 == v.value.i);
  }
  {
    lac_variant v = lac_variant_parse (&ffi_type_sint, VIEW ("012"));
    ensure (&ffi_type_sint == v.type);
    ensure (8 + 2 == v.value.i);
  }
  {
    lac_variant v = lac_variant_parse (&ffi_type_sint, VIEW ("0x12"));
    ensure (&ffi_type_sint == v.type);
    ensure (16 + 2 == v.value.i);
  }

  {
    lac_variant v = lac_variant_parse (&ffi_type_uint32, VIEW ("123"));
    ensure (&ffi_type_uint32 == v.type);
    ensure (123 == v.value.u32);
  }

  {
    lac_variant v = lac_variant_parse (&ffi_type_double, VIEW ("1.23"));
    ensure (&ffi_type_double == v.type);
    ensure (1.23 == v.value.d);
    ensure (lac_variant_address (&v) == &v.value.d);
  }

  return 0;
}

static int incr(int i)
{
	return i+1;
}

int
test_lac_cif ()
{
  {
    ffi_cif cif;
    ffi_type *args[1];
    void *values[1];
    int i = 'a';
	int I;

    /* Initialize the argument info vectors */
    args[0] = &ffi_type_sint;
    values[0] = &i;

    /* Initialize the cif */
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
    if (ffi_prep_cif (&cif, FFI_DEFAULT_ABI, 1, &ffi_type_sint, args) ==
	FFI_OK)
    {
		s = "Hello World!";
		ffi_call (&cif, (void *) puts, &rc, values);
		/* rc now holds the result of the call to puts */

		/* values holds a pointer to the function's arg, so to 
		   call puts() again all we need to do is change the 
		   value of s */
		s = "This is cool!";
		ffi_call (&cif, (void *) puts, &rc, values);
    }
  }
  {
    lac_cif *pcif = lac_cif_alloc (1);
    pcif->sym = puts;
    ffi_type *args[1];
    void *values[1];
    char *s;

    /* Initialize the argument info vectors */
    args[0] = &ffi_type_pointer;
    values[0] = &s;

    /* Initialize the cif */
    ffi_status ret = lac_cif_prep (pcif, &ffi_type_sint, args);
    ensure (FFI_OK == ret);

    s = "Hello World!";
    lac_cif_call (pcif, values);
    s = "This is cool!";
    lac_cif_call (pcif, values);

	lac_cif_free(pcif);
  }
  {
    lac_cif *pcif = lac_cif_alloc (1);
    pcif->sym = printf;
    ffi_type *args[2];
    void *values[2];
    char *s, *fmt;

    /* Initialize the argument info vectors */
    args[0] = &ffi_type_pointer;
    values[0] = &fmt;
	fmt = "%s\n";

    /* Initialize the cif */
    ffi_status ret = lac_cif_prep (pcif, &ffi_type_sint, args);
    ensure (FFI_OK == ret);

	args[1] = &ffi_type_pointer;
	values[1] = &s;
	s = "Hi";
	ret = lac_cif_prep_var(&pcif, 1, &args[1]);
    s = "Hello varargs";

    lac_cif_call (pcif, values);
	ensure (pcif->result.value.i == strlen(s) + 1);

	lac_cif_free(pcif);
  }

  return 0;
}

int
test_lac_ffi ()
{
  test_lac_variant_parse ();
  test_lac_cif ();

  return 0;
}
