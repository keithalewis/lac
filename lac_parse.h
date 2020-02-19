// lac_parse.h - parsing functions
#pragma once
#include <stdio.h>
#include "lac_variant.h"

// tokens are separated by white space (isspace)
// tokens starting with double quote ('"') end at the next quote
//   use '\' to escape '"`
// tokens starting with left curly bracket ('{') end the next
//   right curly bracket ('}') at the same nesting level.
//   use '\' to escape '}`
// returned pointer must be freed
lac_variant lac_parse_token(FILE *);
