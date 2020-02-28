// lac_eval.h - evaluate input streams of characters
#pragma once
#include "lac_ffi.h"

extern lac_variant lac_eval(FILE* fp, ffi_type* type);
extern lac_variant lac_eval_cif(FILE* fp, lac_cif* cif);

extern lac_variant lac_evaluate(FILE* fp);
