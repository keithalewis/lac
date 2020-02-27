/* lac.h - load and call C functions */
#pragma once

#include "ensure.h"
#include "lac_ffi.h"
#include "lac_init.h"
#include "lac_map.h"
//#include "lac_parse.h"
//#include "lac_stack.h"
//#include "lac_stream.h"
#include "lac_variant.h"

extern lac_variant lac_evaluate_type(ffi_type* type, const lac_variant token);
extern lac_variant lac_evaluate_cif(lac_cif* cif, FILE* fp);
extern lac_variant lac_evaluate_block(const lac_variant token);
extern lac_variant lac_evaluate(FILE* fp);
