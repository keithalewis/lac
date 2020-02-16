// lac_parse.h - parsing functions
#pragma once
#include <stdio.h>

// tokens are separated by white space (isspace)
// tokens starting with double quote ('"') end at the next quote quote
//   use '\' to escape '"`
// tokens starting with left curly bracket ('{') end the next
//   right curly bracket ('}') at the same nesting level.
//   use '\' to escape '}`
// returned pointer must be freed
char* lac_parse_token(FILE*);
