// lac_parse.c - parsing functions
#include "ensure.h"
#include "lac_parse.h"
#include "lac_stream.h"

bool token_view_empty(const token_view t)
{
	return t.b == t.e;
}

// return information if error
const char* token_view_error(const token_view t)
{
	return t.e == 0 ? t.b : 0;
}

bool token_view_last(const token_view t)
{
	return EOF == *t.e;
}

int token_view_is_string(const token_view t)
{
	return '"' == *t.b ;
}

int token_view_is_block(const token_view t)
{
	return '{' == *t.b ;
}

#define STREAM(S) _Generic((S), FILE*: (FILE*), lac_stream*: (lac_stream*))

// return first character after white or not space
static int skip(FILE* fp, int(*is)(int), int space)
{
	int c = fgetc(fp);

	while (EOF != c && (space ? is(c) : !is(c))) {
		c = fgetc(fp);
	}

	return c;
}

static char* next_space(FILE* fp, char* b)
{
	int c = fgetc(fp);
	while (EOF != c && !isspace(c)) {
		*b++ = c;
		c = fgetc(fp);
	}
	*b = c; // final character

	return b;
}

// match delimiter at same nesting level
// return pointer past matching right delimiter
// Use b to write buffer.
static char* next_match(FILE* fp, char* b, 
	char l /*= '{'*/, char r /*= '}'*/)
{
	size_t level = 1;

	int c = fgetc(fp);
	while (EOF != c && level != 0) {
		if (c == '\\') {
			*b++ = c;
			c = fgetc(fp);
			ensure (EOF != c);
		}
		else if (c == r) {
			--level;
		}
		else if (c == l) {
			++level;
		}
		*b++ = c;
		c = fgetc(fp);
	}
	*b = c; // final character

	ensure (EOF == c || 0 == level);

	return b;
}
static char* next_quote(FILE* fp, char* b, const char q /*= '"'*/)
{
	return next_match(fp, b, q, q);
}

#define MAX_BUF 1024
// copy stream into static buffer and return view
token_view token_view_next(FILE* fp)
{
	static char buf[MAX_BUF];
	int c;
	char* b = buf;
	char* e;

	c = skip(fp, isspace, true);

	if (EOF == c) {
		return (token_view){0,0}; // empty
	}

	e = buf;
	*e++ = c;
	
	if (c == '"') {
		e = next_quote(fp, e, '"');
	}
	else if (c == '{') {
		e = next_match(fp, e, '{', '}');
	}
	else {
		e = next_space(fp, e);
	}

	/*
	if (e_ == 0) {
		return (token_view){b,0}; // error
	}

	// tokens must be space separated
	if (e_ != e && !isspace(*e_)) {
		return (token_view){b,0};
	}
	*/

	return (token_view){b, e};
}

//
// stream versions
//

// return first character after white or not space
static int stream_skip(lac_stream* fp, int(*is)(int), int space)
{
	int c = lac_stream_getc(fp);

	while (EOF != c && (space ? is(c) : !is(c))) {
		c = lac_stream_getc(fp);
	}

	return c;
}

static char* stream_next_space(lac_stream* fp, char* b)
{
	int c = lac_stream_getc(fp);
	while (EOF != c && !isspace(c)) {
		*b++ = c;
		c = lac_stream_getc(fp);
	}
	*b = c; // final character

	return b;
}

// match delimiter at same nesting level
// return pointer past matching right delimiter
// Use b to write buffer.
static char* stream_next_match(lac_stream* fp, char* b, 
	char l /*= '{'*/, char r /*= '}'*/)
{
	size_t level = 1;

	int c = lac_stream_getc(fp);
	while (EOF != c && level != 0) {
		if (c == '\\') {
			*b++ = c;
			c = lac_stream_getc(fp);
			ensure (EOF != c);
		}
		else if (c == r) {
			--level;
		}
		else if (c == l) {
			++level;
		}
		*b++ = c;
		c = lac_stream_getc(fp);
	}
	*b = c; // final character

	ensure (EOF == c || 0 == level);

	return b;
}
static char* stream_next_quote(lac_stream* fp, char* b, const char q /*= '"'*/)
{
	return stream_next_match(fp, b, q, q);
}

// copy stream into static buffer and return view
token_view token_view_stream_next(lac_stream* fp)
{
	static char buf[MAX_TOKEN_BUF];
	int c;
	char* b = buf;
	char* e;

	c = stream_skip(fp, isspace, true);

	if (EOF == c) {
		return (token_view){0,0}; // empty
	}

	e = buf;
	*e++ = c;
	
	if (c == '"') {
		e = stream_next_quote(fp, e, '"');
	}
	else if (c == '{') {
		e = stream_next_match(fp, e, '{', '}');
	}
	else {
		e = stream_next_space(fp, e);
	}

	/*
	if (e_ == 0) {
		return (token_view){b,0}; // error
	}

	// tokens must be space separated
	if (e_ != e && !isspace(*e_)) {
		return (token_view){b,0};
	}
	*/

	return (token_view){b, e};
}
