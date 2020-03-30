// lac_eval.c
#include "debug.h"
#define _GNU_SOURCE
#include "ensure.h"
#include "lac_cif.h"
#include "lac_eval.h"
#include "lac_map.h"
//#include "lac_parse.h"
#include "lac_token.h"
#include "lac_variant.h"
#include <stdio.h>

#define DEBUG(x) x
/*
// read token from stream and convert to type
lac_variant lac_eval_type(FILE *fp, ffi_type *type)
{
    lac_token t = lac_token_read(fp);
    DEBUG_("type: %s(%c) data: >%s<\n", lac_name(type), t.type, t.data);

    if (t.type == 0) {
        DEBUG_("parse error at: >%s<\n", t.data);
        ensure(t.type != 0);
    }

    if (t.type == EOF) {
        // no more tokens
        return (lac_variant){.type = &ffi_type_void, .value._pointer = NULL};
    }

    lac_variant result;

    // return block
    if (t.type == '{') {
        ensure(type == &ffi_type_pointer);
        result.type = &ffi_type_pointer_malloc;
        result.value._pointer = t.data;

        return result;
    }

    if (t.type == '"') {
        if (type == &ffi_type_pointer) {
            result.type = &ffi_type_pointer_malloc;
            result.value._pointer = t.data;
        }
        else {
            result = lac_variant_scan(type, t.data);
            free(t.data);
        }

        return result;
    }

    const lac_variant *pv = lac_map_get(t.data);

    if (pv) { // in dictionary
        DEBUG_("key: %s\n", t.data);
        if (pv->type == &ffi_type_cif) {
            // ensure (type == &ffi_type_cif || type == &ffi_type_cif_malloc);
            lac_cif *cif = pv->value._pointer;
            result = lac_call_cif(fp, cif);
        }
        else {
            result = *pv; // must not be free'd!!!
            // ensure (pv->type == type);
        }
    }
    else {
        DEBUG_("scan: >%s<\n", t.data);
        result = lac_variant_scan(type, t.data);
        if (type == &ffi_type_pointer) {
            result.type = &ffi_type_pointer_malloc;
        }
        else {
            free(t.data);
        }
    }

    return result;
}

// read arguments from stream and call cif
lac_variant lac_call_cif(FILE *fp, lac_cif *cif)
{
    ffi_cif *ffi = &cif->cif;
    int n = (int)ffi->nargs;
    lac_variant result;
    result.type = ffi->rtype;
    // allocate memory for result based on result.type???

    if (n == 0) {
        ffi_call(ffi, cif->sym, lac_variant_address(&result), NULL);
    }
    else if (n > 0) {
        lac_variant args[n];
        void *addr[n];

        for (int i = 0; i < n; ++i) {
            args[i] = lac_eval_type(fp, ffi->arg_types[i]);
            addr[i] = lac_variant_address(&args[i]);
        }

        ffi_call(ffi, cif->sym, lac_variant_address(&result), addr);

        for (int i = 0; i < n; ++i) {
            if (args[i].type == &ffi_type_pointer_malloc) {
                free(args[i].value._pointer);
            }
        }
    }
    else {
        ensure(n < 0);
        // variadic
    }

    return result;
}

lac_variant lac_eval(FILE *fp)
{
    lac_variant result = {.type = &ffi_type_void, .value._pointer = NULL};

    while (!feof(fp)) {
        result = lac_eval_type(fp, &ffi_type_cif);
    }

    return result;
}

static char* get_token(FILE* fp, char* t, size_t n)
{
    char* u = t;

    // skip whitespace
    int c = getc(fp);
    while (c != EOF && isspace(c)) {
        c = getc(fp);
    }
    while (c != EOF && !isspace(c)) {
        ensure (0 != n--);
        *u++ = c;
        c = getc(fp);
    }
    *u = 0;

    return t;
}
*/
// read arguments from stream and call cif
lac_variant lac_call_cif(FILE *fp, lac_cif *cif)
{
    lac_variant result;

    void* raddr = lac_variant_address(&result);
    ffi_cif *ffi = &cif->cif;
    int n = (int)ffi->nargs;
    result.type = ffi->rtype;
    // allocate memory for result based on result.type???
    // ffi_type knows its size

    if (n == 0) {
        ffi_call(ffi, cif->sym, raddr, NULL);
    }
    else if (n > 0) {
        lac_variant args[n];
        void *addr[n];

        for (int i = 0; i < n; ++i) {
            args[i] = lac_eval_type(fp, ffi->arg_types[i]);
            addr[i] = lac_variant_address(&args[i]);
        }

        ffi_call(ffi, cif->sym, raddr, addr);

        for (int i = 0; i < n; ++i) {
            lac_variant_free(&args[i]);
        }
    }
    else {
        ensure(n < 0);
        // variadic
    }

    return result;
}

// lookup token in dict and call if cif
extern lac_variant lac_eval_s(char* t, FILE* fp)
{
    const lac_variant *pv = lac_map_get(t);

    ensure (pv);

    return pv->type == &ffi_type_cif
        ? lac_call_cif(fp, pv->value._pointer)
        : *pv;

}
extern lac_variant lac_eval(FILE* fp)
{
    lac_variant result;

    lac_token t = lac_token_read(fp);
    // ensure ...
    result = lac_eval_s(t.data, fp);
    lac_token_free(&t);

    return result;
}

// if type is lac_variant use eval
extern lac_variant lac_eval_type_s(char* t, FILE* fp, ffi_type* type)
{
    return type == &ffi_type_variant
        ? lac_eval_s(t, fp)
        : lac_variant_scan(type, t);
}
extern lac_variant lac_eval_type(FILE* fp, ffi_type* type)
{
    lac_variant result;

    lac_token t = lac_token_read(fp);
    // ensure ...
    result = lac_eval_type_s(t.data, fp, type);
    if (type == &ffi_type_pointer) {
        result.type = &ffi_type_pointer_malloc;
    }

    return result;
}
