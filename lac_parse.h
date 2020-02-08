// lac_parse.h - parsing functions
#pragma once
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

// view into token buffer
typedef struct token_view_ {
	const char* b;
	const char* e;
} token_view;

// b == e
bool token_view_empty(const token_view t);

// e == EOF
bool token_view_last(const token_view t);

// return b if e is null
const char* token_view_error(const token_view t);

int token_view_equal(const token_view t, const token_view v);

// b == '"'
int token_view_is_string(const token_view t);

// b == '{'
int token_view_is_block(const token_view t);

// tokens are separated by white space (isspace)
// tokens starting with double quote ('"') end at matching quote
// tokens starting with left curly bracket ('{') end at matching
//   right curly brackets ('}') at the same nesting level.
token_view token_view_next(FILE*);
