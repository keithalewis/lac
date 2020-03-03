// lac_variant.t.c - test lac variant
#define _GNU_SOURCE
#include "ensure.h"
#include "lac_variant.h"

void test_lac_variant_types()
{
#define X(A,B,C,D) \
	lac_variant B ## _ = lac_variant_ ## B(0); \
	ensure (lac_variant_false(B ## _)); \
	ensure (!lac_variant_true(B ## _));
	FFI_TYPE_TABLE(X)
#undef X

#define X(A,B,C,D) \
	ensure (*(A*)lac_variant_address(&B ## _) == 0); \
	ensure (B ## _ .value._pointer == 0);
	FFI_TYPE_TABLE(X)
#undef X

#define X(A,B,C,D) \
	B ## _ . value._ ## B = (A)1; \
	ensure (B ## _ .value._pointer != 0); \
	ensure (!lac_variant_false(B ## _)); \
	ensure (lac_variant_true(B ## _));
	FFI_TYPE_TABLE(X)
#undef X

}

void test_lac_variant_print(const char *out, const lac_variant v)
{
	char *buf;
	size_t size;
	FILE *os = open_memstream(&buf, &size);

	int ret = lac_variant_print(os, v);
	ensure(ret >= 0);
	fclose(os);	

	ensure(0 == strcmp(out, buf));

	free(buf);
}

#define TEST_PARSE(TYPE, STRING, VALUE) \
	{ lac_variant v = lac_variant_parse(&ffi_type_ ## TYPE, STRING); \
	  ensure(VALUE == v.value._ ## TYPE); \
	  test_lac_variant_print(STRING, v); }

int test_lac_variant_parse()
{
	TEST_PARSE(schar, "x", 'x');
	TEST_PARSE(sint, "123", 123);
	TEST_PARSE(double, "1.23", 1.23);
	TEST_PARSE(sint32, "123", 123);
	TEST_PARSE(uint32, "123", 123);
	TEST_PARSE(sint32, "-123", -123);

	return 0;
}

int test_lac_variant_cmp()
{
	lac_variant a, b;

	a.type = &ffi_type_sint;
	a.value._sint = 1;
	b.type = &ffi_type_sint;
	b.value._sint = 2;

	ensure (0 == lac_variant_cmp(a,a));
	ensure (0 > lac_variant_cmp(a,b));
	ensure (0 < lac_variant_cmp(b,a));

	return 0;
}

int test_lac_variant()
{
	test_lac_variant_parse();
	test_lac_variant_types();
	test_lac_variant_cmp();

	return 0;
}
