// lac_parse.t.cpp - test parsing
#include <stdarg.h>
#include <string.h>
#include "ensure.h"
#include "lac_parse.h"
#include "lac_stream.h"

// terminate with null pointer
int test_lac_parse_tokens(char* t, ...)
{
	va_list ap;
	lac_token t0 = LAC_TOKEN(t, 0);
	lac_stream_string_view v = LAC_STREAM_STRING_TOKEN(t0);
	lac_stream s = lac_stream_string(&v);

	lac_token t_;
	va_start(ap, t);
	char* u = va_arg(ap, char*);
	while (u) {
		t_ = lac_stream_token_next(&s);
		ensure (lac_token_equal(t_, LAC_TOKEN(u, 0)));
		u = va_arg(ap, char*);
	}

	t_ = lac_stream_token_next(&s);
	ensure (lac_token_last(t_));

	return 0;
}

int test_lac_parse_alloc()
{
	char tok[] = "tok";
	lac_token t = (lac_token){tok, tok + 3};
	lac_token u = lac_token_alloc(tok, 0);
	ensure (lac_token_equal(t, u));
	lac_token v = lac_token_copy(&u);
	ensure (lac_token_equal(u, v));

	lac_token_free(&v);
	lac_token_free(&u);

	return 0;
}

int test_lac_parse()
{
	test_lac_parse_alloc();
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
