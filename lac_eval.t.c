// lac_eval.t.c - test evaluation
#include "ensure.h"
#include "lac_cif.h"
#include "lac_eval.h"
#include <stdio.h>
#include <string.h>

static inline FILE *is(char *s)
{
    return fmemopen(s, strlen(s), "r");
}

static inline FILE *os(char **s, size_t *n)
{
    return open_memstream(s, n);
}

// call cif by converting string to file stream
static inline lac_variant call_cif(char* s, lac_cif* pcif)
{
    FILE* i = is(s);
    lac_variant v = lac_call_cif(i, pcif);
    fclose(i);

    return v;
}

static int test_lac_eval_cif()
{
    {
        char *s = "\"hello cif\"";
        lac_cif *pcif = lac_cif_alloc(&ffi_type_sint, puts, 1,
                                      (ffi_type *[]){&ffi_type_pointer});
        lac_variant v = call_cif(s, pcif);
        ensure(v.type == &ffi_type_sint);
        ensure(v.value._sint = strlen(s) + 1);
        free(pcif);
    }

    return 0;
}

int test_lac_eval();
extern int test_lac_eval()
{
    test_lac_eval_cif();

    return 0;
}
