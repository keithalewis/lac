// lac_parse.h - parsing functions
#pragma once
#include <ctype.h>
#include <stdbool.h>
#include "lac_stream.h"

// size of largest token
#ifndef MAX_TOKEN_BUF
#define MAX_TOKEN_BUF 1024
#endif

// view into token buffer
typedef struct {
	char* b;
	char* e;
} lac_token;

#define LAC_TOKEN(b, n) (lac_token){b, b + (n ? n : strlen(b))}
lac_token lac_token_alloc(const char*, size_t);
lac_token lac_token_copy(const lac_token*);
void lac_token_free(lac_token*);

int lac_token_size(const lac_token);
int lac_token_equal(const lac_token, const lac_token);

// b == e
bool lac_token_empty(const lac_token t);

// e == EOF
bool lac_token_last(const lac_token t);

// return b if e is null
const char* lac_token_error(const lac_token t);

int lac_token_equal(const lac_token t, const lac_token v);

// b == '"'
int lac_token_is_string(const lac_token t);

// b == '{'
int lac_token_is_block(const lac_token t);

// tokens are separated by white space (isspace)
// tokens starting with double quote ('"') end at matching quote
// tokens starting with left curly bracket ('{') end at matching
//   right curly brackets ('}') at the same nesting level.
lac_token lac_stream_token_next(lac_stream* s);
