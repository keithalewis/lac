// lac_parse.h - parse and input stream into tokens.
#pragma once
#include <stdio.h>

// Tokens are separated by white space (isspace) or end of file.
// Tokens starting with double quote ('"') end at the next double quote.
// Tokens starting with left curly bracket ('{') end at the next
//   right curly bracket ('}') at the same nesting level.
//
// Use backslash '\' to escape any character.
//
// Returned pointer points at n characters.
// If n == EOF then no more tokens
// If n == 0 then parse failed and pointer has the failed parse.
// Returned pointer must be free'd.
char *lac_token_parse(FILE *is, size_t *n /*, size_t* nl */);
