// lac_ffi.t.c - test ffi header
#include <assert.h>
#include <stdio.h>
#include "lac_ffi.h"

#define VIEW(s) s, s + sizeof(s) - 1

int
test_lac_variant_parse ()
{
  {
    lac_variant v = lac_variant_parse (&ffi_type_sint, VIEW ("123"));
    assert (&ffi_type_sint == v.type);
    assert (123 == v.value.i);
    assert (lac_variant_address (&v) == &v.value.i);
  }
  {
    lac_variant v = lac_variant_parse (&ffi_type_sint, VIEW ("123 ") - 1);
    assert (&ffi_type_sint == v.type);
    assert (123 == v.value.i);
  }
  {
    lac_variant v = lac_variant_parse (&ffi_type_sint, VIEW ("123x") - 1);
    assert (&ffi_type_sint == v.type);
    assert (123 == v.value.i);
  }
  {
    lac_variant v = lac_variant_parse (&ffi_type_sint, VIEW ("012"));
    assert (&ffi_type_sint == v.type);
    assert (8 + 2 == v.value.i);
  }
  {
    lac_variant v = lac_variant_parse (&ffi_type_sint, VIEW ("0x12"));
    assert (&ffi_type_sint == v.type);
    assert (16 + 2 == v.value.i);
  }

  {
    lac_variant v = lac_variant_parse (&ffi_type_uint32, VIEW ("123"));
    assert (&ffi_type_uint32 == v.type);
    assert (123 == v.value.u32);
  }

  {
    lac_variant v = lac_variant_parse (&ffi_type_double, VIEW ("1.23"));
    assert (&ffi_type_double == v.type);
    assert (1.23 == v.value.d);
    assert (lac_variant_address (&v) == &v.value.d);
  }

  return 0;
}

int
test_lac_cif ()
{
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
    ffi_arg rc;

    /* Initialize the argument info vectors */
    args[0] = &ffi_type_pointer;
    values[0] = &s;

    /* Initialize the cif */
    ffi_status ret = lac_cif_prep (pcif, &ffi_type_sint, args);
    assert (FFI_OK == ret);

    s = "Hello World!";
    lac_cif_call (pcif, &rc, values);
    s = "This is cool!";
    lac_cif_call (pcif, &rc, values);

	lac_cif_free(pcif);
  }

}

int
test_lac_ffi ()
{
  test_lac_variant_parse ();
  test_lac_cif ();
}
