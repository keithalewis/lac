// lac_parse.h - parsing functions
#pragma once
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

// view into line buffer
typedef struct token_view_ {
	const char* b;
	const char* e;
} token_view;

// b == e
bool token_view_empty(const token_view t);

// return b if e is null
const char* token_view_error(const token_view t);

// tokens are separated by whitespace (isspace) or end of line
// tokens starting with double quote ('"') end at matching quote
// tokens starting with left curly bracket ('{') end at matching
//   right curly brackets ('}') at the same nesting level.
token_view get_token(const char* b, const char* e);
