// lac_parse.h - parse and input stream into tokens.
#pragma once
#include <stdio.h>

// tokens are separated by white space (isspace) or end of file
// tokens starting with double quote ('"') end at the next quote
//   use '\' to escape '"`
// tokens starting with left curly bracket ('{') end the next
//   right curly bracket ('}') at the same nesting level.
//   use '\' to escape '{` and '}'
//
// returned pointer points at n characters
// if n == 0 then no more tokens
// if n == EOF then parse failed
// returned pointer must be free'd
char *lac_token_parse(FILE * is, size_t * n /*, size_t* nl */ );
