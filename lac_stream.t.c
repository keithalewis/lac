// lac_stream.t.c - test lac_stream
#include "ensure.h"
#include "lac_stream.h"

int test_lac_stream()
{
	{
		FILE* ps = tmpfile();
		ensure (0 != ps);
		fputs("ab", ps);
		rewind(ps);

		lac_stream s = lac_stream_file(ps);
		int c;
		c = lac_stream_getc(&s);
		ensure ('a' == c);
		c = lac_stream_getc(&s);
		ensure ('b' == c);
		c = lac_stream_getc(&s);
		ensure (EOF == c);
	}
	{
		lac_stream_string_view sv = LAC_STREAM_STRING_VIEW("ab", 2);

		lac_stream s = lac_stream_string(&sv);
		int c;
		c = lac_stream_getc(&s);
		ensure ('a' == c);
		c = lac_stream_getc(&s);
		ensure ('b' == c);
		c = lac_stream_getc(&s);
		ensure (EOF == c);
	}

	return 0;
}
