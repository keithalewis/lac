// lac_parse.c - parsing functions
#define _GNU_SOURCE
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "lac_parse.h"

// return first character after white (or not) space
static int stream_skip_space(FILE * is)
{
	int c = fgetc(is);

	while (EOF != c && isspace(c)) {
		c = fgetc(is);
	}

	return c;
}

static int stream_next_space(FILE * is, FILE * os)
{
	int c = fgetc(is);

	while (EOF != c && !isspace(c)) {
		if (c == '\\') {
			// escape backslash
			c = fgetc(is);
			if (EOF == c) {
				return EOF;
			}
		}
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
	while (EOF != c) {
		if (c == '\\') {
			c = fgetc(is);
			if (EOF == c) {
				return 0; // error
			}
		} else if (c == r) {
			--level;
		} else if (c == l) {
			++level;
		}
		if (0 == level) {
			break;
		}
		fputc(c, os);
		c = fgetc(is);
	}

	if (0 != level) {
		return 0; // error
	}

	return c;
}

static int stream_next_quote(FILE * is, FILE * os, const char q /*= '"'*/ )
{
	return stream_next_match(is, os, q, q);
}

// must call free on return pointer
char* lac_token_parse(FILE* is, size_t *n)
{
	int c;

	c = stream_skip_space(is);

	if (EOF == c) {
		// last token
		*n = 0;
		return "";
	}

	char *buf;
	FILE *os = open_memstream(&buf, n);

	int ret;
	if (c == '"') {
		ret = stream_next_quote(is, os, '"');
		if (ret != '"' || ret == 0) {
			*n = EOF; // error
		}
	} else if (c == '{') {
		ret = stream_next_match(is, os, '{', '}');
		if (ret != '}' || ret == 0) {
			*n = EOF; // error
		}
	} else {
		fputc(c, os);
		ret = stream_next_space(is, os);
		if (!isspace(ret) || ret == 0) {
			*n = EOF; // error
		}
	}

	// add null terminator
	fclose(os);

	return buf;
}
