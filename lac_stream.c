// lac_stream.c
#include "lac_stream.h"

int lac_stream_getc(lac_stream* ps)
{
	return ps->getc(ps->p);
}

int lac_stream_file_getc(void* p)
{
	return fgetc(p);
}

int lac_stream_string_getc(void* p)
{
	lac_stream_string_view* ps = p;

	return ps->i < ps->e ? *ps->i++ : EOF;
}

lac_stream lac_stream_file(FILE *fp)
{
	return (lac_stream){fp, lac_stream_file_getc};
}

lac_stream lac_stream_string(lac_stream_string_view *sp)
{
	return (lac_stream){sp, lac_stream_string_getc};
}
