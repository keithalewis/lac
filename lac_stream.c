// lac_stream.c
#include "lac_stream.h"

int lac_getc(lac_stream* ps)
{
	if (ps->i < ps->e) {
		return *ps->i++;
	}
	
	return EOF;
}
