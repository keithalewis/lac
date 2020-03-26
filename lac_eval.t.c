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

static int test_lac_eval_type()
{
    {
        char* s = "hello";

        FILE* i = is(s);
        lac_variant v = lac_eval_type(i, &ffi_type_pointer);
        fclose(i);

        ensure (v.type == &ffi_type_pointer_malloc);
        ensure (0 == strcmp(v.value._pointer, s));
        free (v.value._pointer);
    }
    {
        char* s = "\"hello\"";

        FILE* i = is(s);
        lac_variant v = lac_eval_type(i, &ffi_type_pointer);
        fclose(i);

        ensure (v.type == &ffi_type_pointer_malloc);
        ensure (0 == strcmp(v.value._pointer, "hello"));
        free (v.value._pointer);
    }
    {
        char* s = "{hello}";

        FILE* i = is(s);
        lac_variant v = lac_eval_type(i, &ffi_type_pointer);
        fclose(i);

        ensure (v.type == &ffi_type_pointer_malloc);
        ensure (0 == strcmp(v.value._pointer, "hello"));
        free (v.value._pointer);
    }
    {
        char* s = "123";

        FILE* i = is(s);
        lac_variant v = lac_eval_type(i, &ffi_type_sint);
        fclose(i);

        ensure (v.type == &ffi_type_sint);
        ensure (v.value._sint == 123);
    }
    {
        char* s = "123";

        FILE* i = is(s);
        lac_variant v = lac_eval_type(i, &ffi_type_uint);
        fclose(i);

        ensure (v.type == &ffi_type_uint);
        ensure (v.value._uint == 123);
    }
    {
        char* s = "1.23";

        FILE* i = is(s);
        lac_variant v = lac_eval_type(i, &ffi_type_double);
        fclose(i);

        ensure (v.type == &ffi_type_double);
        ensure (v.value._double == 1.23);
    }
    {
        char* s = "1.5";

        FILE* i = is(s);
        lac_variant v = lac_eval_type(i, &ffi_type_float);
        fclose(i);

        ensure (v.type == &ffi_type_float);
        ensure (v.value._float == 1.5);
    }
    
    return 0;
}

int test_lac_eval();
extern int test_lac_eval()
{
    test_lac_eval_cif();
    test_lac_eval_type();

    return 0;
}
