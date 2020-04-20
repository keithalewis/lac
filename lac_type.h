// lac_type.h - extend ffi_type
#pragma once
#include <stdlib.h>
#include <string.h>
#include <ffi.h>

typedef struct {
    ffi_type type;
    ffi_type* elements[1]; // storage for type.elements array
} lac_type;

static inline lac_type* lac_type_struct(size_t n, const ffi_type* elements)
{
    lac_type* p = malloc(sizeof(lac_type) + (n + 1)*sizeof(ffi_type));
    if (!p) {
        return p;
    }
    
    p->type.type = FFI_TYPE_STRUCT;
    p->type.size = 0;
    p->type.alignment = 0; // set by ffi_prep_cif
    p->type.elements = p->elements;
    if (elements) { // copy all or nothing
        memcpy(p->elements, elements, n*sizeof(ffi_type*));
    }
    p->elements[n] = NULL; // used by ffi_prep_cif

    return p;
}
static inline void lac_type_free(lac_type* p)
{
    free (p);
}

// non-allocating
static inline lac_type lac_type_make(ffi_type* type)
{
    lac_type t;

    t.type = *type;
    t.type.elements = t.elements;
    t.elements[0] = type->elements[0];

    return t;
}
