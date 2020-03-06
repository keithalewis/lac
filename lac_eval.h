// lac_eval.h - evaluate input streams of characters
#pragma once
#include <stdio.h>
#include "lac_cif.h"

// Evaluate tokens on input stream based on required type.
extern lac_variant lac_eval(FILE * fp, ffi_type * type);

extern lac_variant lac_call_cif(FILE * fp, lac_cif * cif);
extern lac_variant lac_eval_type(FILE * fp, ffi_type * type);
