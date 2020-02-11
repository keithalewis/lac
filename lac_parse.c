// lac_parse.c - parsing functions
#include "ensure.h"
#include "lac_stream.h"
#include "lac_parse.h"

bool lac_token_empty(const lac_token t)
{
	return t.b == t.e;
}

// return information if error
const char* lac_token_error(const lac_token t)
{
	return t.e == 0 ? t.b : 0;
}

int lac_token_size(const lac_token t)
{
	return t.e - t.b;
}

int lac_token_equal(const lac_token t, const lac_token u)
{
	int n = lac_token_size(t);
	
	return n == lac_token_size(u) && 0 == strncmp(t.b, u.b, n);
}

bool lac_token_last(const lac_token t)
{
	return EOF == *t.e;
}

int lac_token_is_string(const lac_token t)
{
	return '"' == *t.b ;
}

int lac_token_is_block(const lac_token t)
{
	return '{' == *t.b ;
}

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
lac_token lac_stream_token_next(lac_stream* fp)
{
	static char buf[MAX_TOKEN_BUF];
	int c;
	char* b = buf;
	char* e = buf;

	c = stream_skip(fp, isspace, true);

	if (EOF == c) {
		*e = EOF;

		return (lac_token){b,e};
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
		return (lac_token){b,0}; // error
	}

	// tokens must be space separated
	if (e_ != e && !isspace(*e_)) {
		return (lac_token){b,0};
	}
	*/

	return (lac_token){b, e};
}
