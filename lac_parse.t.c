// lac_parse.t.cpp - test parsing
#define _GNU_SOURCE
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ensure.h"
#include "lac_variant.h"

// terminate with null pointer
int test_lac_token_parse(char *t, ...)
{
	FILE *s = fmemopen(t, strlen(t), "r");

	va_list ap;
	va_start(ap, t);
	lac_variant t_;
	char *u = va_arg(ap, char *);
	while (u) {
		t_ = lac_token_parse(s);
		ensure(0 == strcmp(t_.value._pointer, u));
		free(t_.value._pointer);
		u = va_arg(ap, char *);
	}

	t_ = lac_token_parse(s);
	ensure(t_.type == &ffi_type_void);

	fclose(s);

	return 0;
}

int test_lac_convert()
{
	{
		lac_variant w = lac_variant_parse(&ffi_type_sint, "123");
		ensure (w.type = &ffi_type_sint);
		ensure (w.value._sint == 123);
	}
	{
		lac_variant w = lac_variant_parse(&ffi_type_double, "1.23");
		ensure (w.type = &ffi_type_double);
		ensure (w.value._double == 1.23);
	}
	{
		lac_variant w = lac_variant_parse(&ffi_type_string, "foo bar");
		ensure (w.type = &ffi_type_string);
		ensure (0 == strcmp(w.value._pointer, "foo bar"));
	}

	return 0;
}

int test_lac_parse()
{
	test_lac_convert();

	test_lac_token_parse("a b c", "a", "b", "c", 0);
	test_lac_token_parse("a\tb c", "a", "b", "c", 0);
	test_lac_token_parse("a\tb\r c", "a", "b", "c", 0);
	test_lac_token_parse("a\tb\r \nc", "a", "b", "c", 0);
	test_lac_token_parse("{a {b} c}", "a {b} c", 0);
	test_lac_token_parse(" {a {b} c}  d", "a {b} c", "d", 0);
	test_lac_token_parse("%g\n", "%g", 0);
	test_lac_token_parse("\"%g\n\" h", "%g\n", "h", 0);

	return 0;
}
