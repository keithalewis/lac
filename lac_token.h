// lac_token.h - read string token from input stream
#pragma once
#include <stdlib.h>
#include <stdio.h>

// Tokens are separated by white space (isspace) or end of file.
// Tokens starting with double quote ('"') end at the next double quote.
// Tokens starting with left curly bracket ('{') end at the next
//   right curly bracket ('}') at the same nesting level.
//
// Use backslash '\' to escape quote or brace character.
//
// Returned pointer points at n characters.
// If type == EOF then no more tokens
// If type == 0 then parse failed and pointer has the failed parse.
// Returned pointer must be free'd.

typedef struct {
    int type;    // initial non whitespace character;
    char *data;  // token characters
    size_t size; // number of characters read
} lac_token;

extern lac_token lac_token_read(FILE *is /*, size_t* nl */);
static inline void lac_token_free(lac_token* p)
{
    free (p->data);
}
