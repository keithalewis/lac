// lac_parse.t.cpp - test parsing
#include <stdarg.h>
#include <string.h>
#include "ensure.h"
#include "lac_parse.h"

// terminate with null pointer
int test_lac_parse_tokens(char* t, ...)
{
	FILE* s = fmemopen(t, strlen(t), "r");

	va_list ap;
	va_start(ap, t);
	char* t_;
	char* u = va_arg(ap, char*);
	while (u) {
		t_ = lac_parse_token(s);
		ensure (0 == strcmp(t_, u));
		u = va_arg(ap, char*);
	}

	t_ = lac_parse_token(s);
	ensure (t_ == 0);

	return 0;
}

int test_lac_parse()
{
	test_lac_parse_tokens("a b c", "a", "b", "c", 0);
	test_lac_parse_tokens("a\tb c", "a", "b", "c", 0);
	test_lac_parse_tokens("a\tb\r c", "a", "b", "c", 0);
	test_lac_parse_tokens("a\tb\r \nc", "a", "b", "c", 0);
	test_lac_parse_tokens("\"b \" \"", "\"b \"", 0);
	test_lac_parse_tokens("{a {b} c}", "{a {b} c}", 0);
	test_lac_parse_tokens(" {a {b} c}  d", "{a {b} c}", "d", 0);
	test_lac_parse_tokens("%g\n", "%g", 0); // ???
	/*
	{
		lac_token v = LAC_TOKEN("\"b c");

		t = lac_token_next(v.b, v.e);
		assert (lac_token_error(t));
	}
	{
		lac_token v = LAC_TOKEN("\"b \"c ");

		t = lac_token_next(v.b, v.e);
		// missing whitespace after matching quote
		assert (lac_token_error(t));
		assert (0 == strncmp(v.b, "\"b \"c", 5));
	}
		lac_token v = LAC_TOKEN("{a {b} c");

		t = lac_token_next(v.b, v.e);
		assert (lac_token_error(t));
	}
	{
	}
	*/

	return 0;
}
