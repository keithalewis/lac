// lac_parse.t.cpp - test parsing
#include <assert.h>
#include <string.h>
#include "lac_parse.h"

#define VIEW(s) s, s + strlen(s)

int token_view_equal(const token_view t, const token_view v)
{
	if (t.e - t.b != v.e - v.b) {
		return 0;
	}
	
	return !strncmp(t.b, v.b, t.e - t.b);
}
int test_lac_parse()
{
	token_view t;

	{
		token_view v = { VIEW("a b c") };
		t = get_token(v.b, v.e);
		assert (0 == strncmp(t.b, "a", 1));
		t = get_token(t.e, v.e);
		assert (0 == strncmp(t.b, "b", 1));
		t = get_token(t.e, v.e);
		assert (0 == strncmp(t.b, "c", 1));
		t = get_token(t.e, v.e);
		assert (token_view_empty(t));
	}
	{
		token_view v = { VIEW("a\tb c") };

		t = get_token(v.b, v.e);
		assert (0 == strncmp(t.b, "a", 1));
		t = get_token(t.e, v.e);
		assert (0 == strncmp(t.b, "b", 1));
		t = get_token(t.e, v.e);
		assert (0 == strncmp(t.b, "c", 1));
		t = get_token(t.e, v.e);
		assert (token_view_empty(t));
	}
	{
		token_view v = { VIEW("\"b c") };

		t = get_token(v.b, v.e);
		assert (token_view_error(t));
	}
	{
		token_view v = { VIEW("\"b \"c") };

		t = get_token(v.b, v.e);
		assert (token_view_equal(t,v));
	}

	return 0;
}
