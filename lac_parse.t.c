// lac_parse.t.cpp - test parsing
#include <assert.h>
#include <string.h>
#include "lac_parse.h"
#include "lac_stream.h"

int test_lac_parse()
{
	{
		lac_token t = LAC_TOKEN("a b c", 0);
		lac_stream_string_view v = LAC_STREAM_STRING_TOKEN(t);
		lac_stream s = lac_stream_string(&v);
		t = lac_stream_token_next(&s);
		assert (0 == strncmp(t.b, "a", 1));
		t = lac_stream_token_next(&s);
		assert (0 == strncmp(t.b, "b", 1));
		t = lac_stream_token_next(&s);
		assert (0 == strncmp(t.b, "c", 1));
		t = lac_stream_token_next(&s);
		assert (lac_token_empty(t));
	}
	/*
	{
		lac_token v = LAC_TOKEN("a\tb c");

		t = lac_token_next(v.b, v.e);
		assert (0 == strncmp(t.b, "a", 1));
		t = lac_token_next(t.e, v.e);
		assert (0 == strncmp(t.b, "b", 1));
		t = lac_token_next(t.e, v.e);
		assert (0 == strncmp(t.b, "c", 1));
		t = lac_token_next(t.e, v.e);
		assert (lac_token_empty(t));
	}
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
	{
		lac_token v = LAC_TOKEN("\"b \" \"");

		t = lac_token_next(v.b, v.e);
		assert (lac_token_equal(t, LAC_TOKEN("\"b \"")));
	}
	{
		lac_token v = LAC_TOKEN("{a {b} c");

		t = lac_token_next(v.b, v.e);
		assert (lac_token_error(t));
	}
	{
		lac_token v = LAC_TOKEN("{a {b} c}");

		t = lac_token_next(v.b, v.e);
		assert (lac_token_equal(t, v));
	}
	*/

	return 0;
}
