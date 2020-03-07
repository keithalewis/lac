// lac_parse.c - parsing functions
#define _GNU_SOURCE
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "lac_parse.h"

// return first character after white space
static int stream_skip_space(FILE * is)
{
	int c = fgetc(is);

	while (EOF != c && isspace(c)) {
		c = fgetc(is);
	}

	return c;
}

// write non space characters to output steam and escape backslash
static int stream_next_space(FILE * is, FILE * os)
{
	int c = fgetc(is);

	while (c != EOF && !isspace(c)) {
		if (c == '\\') {
			// escape white space
			int c_ = fgetc(is);

			if (c_ == EOF) {
				fputc(c, os);

				return EOF;
			} else if (!isspace(c_)) {
				fputc(c, os);
			}

			c = c_;
		}
		fputc(c, os);
		c = fgetc(is);
	}

	return c;
}

// match delimiters at same nesting level and escape backslash
static int stream_next_match(FILE * is, FILE * os,
			     char l /*= '{'*/ , char r /*= '}'*/ )
{
	size_t level = 1;

	int c = fgetc(is);
	while (level && c != EOF) {
		if (c == r) {
			--level;
		} else if (c == l) {
			++level;
		} else if (c == '\\') {
			int c_ = fgetc(is);

			if (c_ == EOF) {
				fputc(c, os);

				// assert (level != 0);
				return 0;	// error
			} else if (c_ != l && c_ != r) {
				fputc(c, os);
			}

			c = c_;
		}
		// don't include right delimiter
		if (level != 0) {
			fputc(c, os);
		} else {
			break;
		}

		c = fgetc(is);
	}

	if (0 != level) {
		return 0;	// error
	}

	return c;
}

static int stream_next_quote(FILE * is, FILE * os, const char q /*= '"'*/ )
{
	return stream_next_match(is, os, q, q);
}

// must call free on return pointer if n > 0
char *lac_token_parse(FILE * is, size_t * n)
{
	int c;
	char *buf;
	FILE *os = open_memstream(&buf, n);

	c = stream_skip_space(is);

	// escape backslash
	if (c == '\\') {
		int c_ = fgetc(is);
		if (c_ == EOF) {
			fputc(c, os);
		} 
		else  {
			fputc(c_, os);
			c_ = fgetc(is);
		}
		c = c_;
	}

	if (c == EOF) {
		fclose(os);

		return buf;
	}

	int ret;

	if (c == '"') {
		ret = stream_next_quote(is, os, '"');
		if (ret != '"') {
			ret = 0;	// error
		}
	} else if (c == '{') {
		ret = stream_next_match(is, os, '{', '}');
		if (ret != '}') {
			ret = 0;	// error
		}
	} else {
		fputc(c, os);
		ret = stream_next_space(is, os);
		if (!isspace(ret) && ret != EOF) {
			ret = 0;	// error
		}
	}

	fclose(os);		// sets *n

	if (ret == 0) {
		*n = 0;	// parse failed
	}

	return buf;
}
