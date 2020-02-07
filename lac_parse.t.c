// lac_parse.t.cpp - test parsing
#include <assert.h>
#include <string.h>
#include "lac_parse.h"

#define VIEW(s) (token_view){s, s + strlen(s)}

int token_view_equal(const token_view t, const token_view v)
{
	if (t.e - t.b != v.e - v.b) {
		return 0;
	}
	
	return !strncmp(t.b, v.b, t.e - t.b);
}

int test_lac_parse()
{
	return 0;
}
/*
	token_view t;

	{
		token_view v = VIEW("a b c");
		t = token_view_next(v.b, v.e);
		assert (0 == strncmp(t.b, "a", 1));
		t = token_view_next(t.e, v.e);
		assert (0 == strncmp(t.b, "b", 1));
		t = token_view_next(t.e, v.e);
		assert (0 == strncmp(t.b, "c", 1));
		t = token_view_next(t.e, v.e);
		assert (token_view_empty(t));
	}
	{
		token_view v = VIEW("a\tb c");

		t = token_view_next(v.b, v.e);
		assert (0 == strncmp(t.b, "a", 1));
		t = token_view_next(t.e, v.e);
		assert (0 == strncmp(t.b, "b", 1));
		t = token_view_next(t.e, v.e);
		assert (0 == strncmp(t.b, "c", 1));
		t = token_view_next(t.e, v.e);
		assert (token_view_empty(t));
	}
	{
		token_view v = VIEW("\"b c");

		t = token_view_next(v.b, v.e);
		assert (token_view_error(t));
	}
	{
		token_view v = VIEW("\"b \"c ");

		t = token_view_next(v.b, v.e);
		// missing whitespace after matching quote
		assert (token_view_error(t));
		assert (0 == strncmp(v.b, "\"b \"c", 5));
	}
	{
		token_view v = VIEW("\"b \" \"");

		t = token_view_next(v.b, v.e);
		assert (token_view_equal(t, VIEW("\"b \"")));
	}
	{
		token_view v = VIEW("{a {b} c");

		t = token_view_next(v.b, v.e);
		assert (token_view_error(t));
	}
	{
		token_view v = VIEW("{a {b} c}");

		t = token_view_next(v.b, v.e);
		assert (token_view_equal(t, v));
	}

	return 0;
}
*/
