// lac_eval.h - evaluate input streams of characters
#pragma once
#include "lac_ffi.h"

extern lac_variant lac_evaluate_type(ffi_type* type, const lac_variant token);
extern lac_variant lac_evaluate_cif(lac_cif* cif, FILE* fp);
extern lac_variant lac_evaluate_block(const lac_variant token);
extern lac_variant lac_evaluate(FILE* fp);
