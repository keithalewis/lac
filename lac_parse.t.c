// lac_parse.t.cpp - test parsing
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "ensure.h"
#include "lac_variant.h"

// terminate with null pointer
int test_lac_parse_tokens(char *t, ...)
{
	FILE *s = fmemopen(t, strlen(t), "r");

	va_list ap;
	va_start(ap, t);
	lac_variant t_;
	char *u = va_arg(ap, char *);
	while (u) {
		t_ = lac_parse_token(s);
		ensure(0 == strcmp(t_.value._pointer, u));
		lac_variant_free(&t_);
		u = va_arg(ap, char *);
	}

	t_ = lac_parse_token(s);
	ensure(t_.type == &ffi_type_void);

	fclose(s);

	return 0;
}

int test_lac_convert()
{
	{
		char buf[] = "123";
		FILE *s = fmemopen(buf, sizeof(buf), "r");

		lac_variant v = lac_parse_token(s);
		ensure (v.type = &ffi_type_string);
		ensure (0 == strcmp(v.value._pointer, buf));
		lac_variant w = lac_variant_parse(&ffi_type_sint, &v);
		ensure (w.type = &ffi_type_sint);
		ensure (w.value._sint == 123);

		fclose(s);
	}
	{
		char buf[] = "1.23";
		FILE *s = fmemopen(buf, sizeof(buf), "r");

		lac_variant v = lac_parse_token(s);
		ensure (v.type = &ffi_type_string);
		ensure (0 == strcmp(v.value._pointer, buf));
		lac_variant w = lac_variant_parse(&ffi_type_double, &v);
		ensure (w.type = &ffi_type_double);
		ensure (w.value._double == 1.23);

		fclose(s);
	}
	{
		char buf[] = "1.23";
		FILE *s = fmemopen(buf, sizeof(buf), "r");

		lac_variant v = lac_parse_token(s);
		ensure (v.type = &ffi_type_string);
		ensure (0 == strcmp(v.value._pointer, buf));
		lac_variant w = lac_variant_parse(&ffi_type_string, &v);
		ensure (w.type = &ffi_type_string);
		ensure (0 == strcmp(v.value._pointer, buf));

		lac_variant_free(&v);

		fclose(s);
	}

	return 0;
}

int test_lac_parse()
{
	test_lac_convert();

	test_lac_parse_tokens("a b c", "a", "b", "c", 0);
	test_lac_parse_tokens("a\tb c", "a", "b", "c", 0);
	test_lac_parse_tokens("a\tb\r c", "a", "b", "c", 0);
	test_lac_parse_tokens("a\tb\r \nc", "a", "b", "c", 0);
	test_lac_parse_tokens("{a {b} c}", "a {b} c", 0);
	test_lac_parse_tokens(" {a {b} c}  d", "a {b} c", "d", 0);
	test_lac_parse_tokens("%g\n", "%g", 0);
	test_lac_parse_tokens("\"%g\n\" h", "%g\n", "h", 0);

	return 0;
}
