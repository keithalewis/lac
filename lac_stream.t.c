// lac_stream.t.c - test lac_stream
#include "ensure.h"
#include "lac_stream.h"

int test_lac_stream()
{
	{
		lac_stream s = LAC_STREAM("abc", 0);
		lac_stream* ps = &s;
		int c;

		c = sgetc(ps);
		ensure ('a' == c);
		c = sgetc(ps);
		ensure ('b' == c);
		c = sgetc(ps);
		ensure ('c' == c);
		c = sgetc(ps);
		ensure (EOF == c);
	}
	{
		FILE* ps = tmpfile();
		ensure (0 != ps);

		int n;
		n = fputs("abc", ps);
		//ensure (3 == fputs("abc", ps));
		int c;

		rewind(ps);
		c = sgetc(ps);
		ensure ('a' == c);
		c = sgetc(ps);
		ensure ('b' == c);
		c = sgetc(ps);
		ensure ('c' == c);
		c = sgetc(ps);
		ensure (EOF == c);
	}

	return 0;
}
