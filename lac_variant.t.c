// lac_variant.t.c - test lac variant
#include "ensure.h"
#include "lac_variant.h"

void
test_lac_variant_scan(char* in, lac_variant* pv)
{
	FILE* is = fmemopen(in, strlen(in), "r");

	int ret = lac_variant_scan(is, pv);
	ensure (ret !=0 && ret != EOF);

	fclose(is);
}

void
test_lac_variant_print(const char* out, const lac_variant v)
{
	char* buf;
	size_t size;
	FILE* os = open_memstream(&buf, &size);

	int ret = lac_variant_print(os, v);
	ensure (ret >= 0);
	ensure (0 == strcmp(out, buf));

	fclose(os);
	free(buf);
}

int
test_lac_variant_parse()
{
	{
		lac_variant v;
		v.type = &ffi_type_sint;
		test_lac_variant_scan("123", &v);

		ensure (v.value.i == 123);
		test_lac_variant_print("123", v);
	}
	{
		lac_variant v;
		v.type = &ffi_type_double;
		test_lac_variant_scan("1.23", &v);

		ensure (v.value.g == 1.23);
		test_lac_variant_print("1.23", v);
	}

  return 0;
}

int test_ffi_type_format()
{
	ensure ('i' == ffi_type_format(&ffi_type_sint));
	ensure ('u' == ffi_type_format(&ffi_type_uint));
	ensure ('g' == ffi_type_format(&ffi_type_double));
	ensure ('f' == ffi_type_format(&ffi_type_float));
	ensure ('p' == ffi_type_format(&ffi_type_pointer));

	return 0;
}
/*
int test_lac_variant_union_prep()
{
	ffi_type_variant_prep();

	return 0;
}
*/

int test_lac_variant()
{
	test_lac_variant_parse();
	test_ffi_type_format();
//	test_lac_variant_union_prep();

	return 0;
}
