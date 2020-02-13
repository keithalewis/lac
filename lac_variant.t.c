// lac_variant.t.c - test lac variant
#include "ensure.h"
#include "lac_variant.h"

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

int test_ffi_type_format()
{
	ensure ('i' == ffi_type_format(&ffi_type_sint));
	ensure ('u' == ffi_type_format(&ffi_type_uint));
	ensure ('d' == ffi_type_format(&ffi_type_double));
	ensure ('f' == ffi_type_format(&ffi_type_float));
	ensure ('p' == ffi_type_format(&ffi_type_pointer));

	return 0;
}

int test_lac_variant_union_prep()
{
	prep_variant_union_type();

	return 0;
}

int test_lac_variant()
{
	test_lac_variant_parse();
	test_ffi_type_format();
	test_lac_variant_union_prep();

	return 0;
}
