// lac_eval.h - evaluate input streams of characters
#pragma once
#include "lac_cif.h"
#include <stdio.h>
#include <string.h>

// Evaluate tokens on input stream based on required type.
extern lac_variant lac_eval(FILE *fp);

extern lac_variant lac_call_cif(FILE *fp, lac_cif *cif);
extern lac_variant lac_eval_type(FILE *fp, ffi_type *type);

static inline lac_variant lac_eval_str(char *is)
{
    FILE *fp;
    lac_variant result;

    fp = fmemopen(is, strlen(is), "r");
    result = lac_eval(fp);
    fclose(fp);

    return result;
}

extern lac_variant lac_eval_s(FILE*);
extern lac_variant lac_call_cif_s(FILE *fp, lac_cif *cif);
extern lac_variant lac_eval_type_s(FILE*, ffi_type*);
