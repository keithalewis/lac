// lac_eval.h - evaluate input streams of characters
#pragma once
#include <stdio.h>
#include "lac_ffi.h"

// Evaluate tokens on input stream based on required type.
extern lac_variant lac_eval(FILE* fp, ffi_type* type);
