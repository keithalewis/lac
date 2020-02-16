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
	fclose(os); // fflush out

	ensure (0 == strcmp(out, buf));

	free(buf);
}

#define TEST_PARSE(TYPE, STRING, VALUE) \
	{ lac_variant v; v.type = &ffi_type_ ## TYPE; test_lac_variant_scan(STRING, &v); \
	  ensure(VALUE == v.value._ ## TYPE); test_lac_variant_print(STRING, v); }

int
test_lac_variant_parse()
{
  TEST_PARSE(schar, "x", 'x');
  TEST_PARSE(sint, "123", 123);
  TEST_PARSE(double, "1.23", 1.23);

  return 0;
}

int test_lac_variant()
{
	test_lac_variant_parse();

	return 0;
}
