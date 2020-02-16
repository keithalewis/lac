// lac_parse.c - parsing functions
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "ensure.h"
#include "lac_parse.h"

// return first character after white or not space
static int stream_skip(FILE * is, int (*isa)(int), int space)
{
	int c = fgetc(is);

	while (EOF != c && (space ? isa(c) : !isa(c))) {
		c = fgetc(is);
	}

	return c;
}

static int stream_next_space(FILE * is, FILE * os)
{
	int c = fgetc(is);

	while (EOF != c && !isspace(c)) {
		fputc(c, os);
		c = fgetc(is);
	}

	return c;
}

// match delimiter at same nesting level
// return pointer past matching right delimiter
// Use b to write buffer.
static int stream_next_match(FILE * is, FILE * os,
			     char l /*= '{'*/ , char r /*= '}'*/ )
{
	size_t level = 1;

	int c = fgetc(is);
	while (EOF != c && level != 0) {
		if (c == '\\') {
			fputc(c, os);
			c = fgetc(is);
			ensure(EOF != c);
		} else if (c == r) {
			--level;
		} else if (c == l) {
			++level;
		}
		fputc(c, os);
		c = fgetc(is);
	}

	ensure(0 == level);

	return c;
}

static int stream_next_quote(FILE * is, FILE * os, const char q /*= '"'*/ )
{
	return stream_next_match(is, os, q, q);
}

// copy stream into static buffer and return view
char *lac_parse_token(FILE * is)
{
	int c;

	c = stream_skip(is, isspace, true);

	if (EOF == c) {
		return 0;
	}

	char *buf;
	size_t size;
	FILE *os = open_memstream(&buf, &size);

	fputc(c, os);

	if (c == '"') {
		stream_next_quote(is, os, '"');
	} else if (c == '{') {
		stream_next_match(is, os, '{', '}');
	} else {
		stream_next_space(is, os);
	}

	fclose(os);

	return buf;
}
