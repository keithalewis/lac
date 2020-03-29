// lac_init.c - initialize lac
#include "lac_init.h"
#include "ensure.h"
#include "lac_cif.h"
#include "lac_eval.h"
#include "lac_map.h"
#include <dlfcn.h>
#include <stdbool.h>

// pointed to value must exist
static void put_(char *key, const lac_variant val)
{
    lac_variant *pv = lac_variant_alloc();

    ensure(pv);
    *pv = val; // pointers must exist

    lac_map_put(key, pv);
}

static lac_variant get_(const char *key)
{
    const lac_variant *val = lac_map_get(key);

    if (!val) {
        return (lac_variant){.type = &ffi_type_void, .value._pointer = 0};
    }

    return *val;
}

static lac_variant parse_(ffi_type *type, char *s)
{
    return lac_variant_scan(type, s);
}

static ffi_type *double_(void)
{
    return &ffi_type_double;
}

static int print_(const lac_variant pv)
{
    return lac_variant_print(stdout, &pv);
}

static void nl_(void)
{
    printf("\n");
}

static void tab_(void)
{
    printf("\t");
}

#define put_ffi_type(name)                                                     \
    static lac_variant name##_;                                                \
    name##_.type = &ffi_type_pointer;                                          \
    name##_.value._pointer = &ffi_type_##name;                                 \
    lac_map_put(#name, &name##_);

#define put_pointer(name)                                                      \
    static lac_variant name##_;                                                \
    name##_.type = &ffi_type_pointer;                                          \
    name##_.value._pointer = name;                                             \
    lac_map_put(#name, &name##_);

#define put_int(name)                                                          \
    static lac_variant name##_;                                                \
    name##_.type = &ffi_type_sint;                                             \
    name##_.value._sint = name;                                                \
    lac_map_put(#name, &name##_);

#define put_cif(name, cif_ptr)                                                 \
    static lac_variant name##_cif;                                             \
    name##_cif.type = &ffi_type_cif;                                           \
    name##_cif.value._pointer = cif_ptr;                                       \
    lac_map_put(#name, &name##_cif);

static void print_entry(const char *key, const void *val)
{
    const lac_variant *v = val;
    printf("%9s : %8s\n", key, lac_name(v->type)); // !!!use fnmatch()
}

static void print_map(void)
{
    lac_map_foreach(print_entry);
}

// while {cond} {body}
static void while_(char *cond, char *body)
{
    lac_variant vcond;

    FILE *pcond = fmemopen(cond, strlen(cond), "r");
    FILE *pbody = fmemopen(body, strlen(body), "r");

    vcond = lac_eval(pcond);
    while (lac_variant_true(vcond)) {
        lac_eval(pbody);
        rewind(pbody);
        rewind(pcond);
        vcond = lac_eval(pcond);
    }
}

void lac_init(void)
{
    ffi_type_variant_prep();

    ffi_type *type[8];

    put_int(true);
    put_int(false);

    put_pointer(stdin);
    put_pointer(stdout);
    put_pointer(stderr);

    type[0] = &ffi_type_pointer;
    type[1] = &ffi_type_variant;
    put_cif(put, lac_cif_alloc(&ffi_type_void, put_, 2, type));

    type[0] = &ffi_type_pointer;
    put_cif(get, lac_cif_alloc(&ffi_type_variant, get_, 1, type));

    put_int(RTLD_LAZY);
    put_int(RTLD_NOW);

    type[0] = &ffi_type_pointer;
    type[1] = &ffi_type_sint;
    put_cif(dlopen, lac_cif_alloc(&ffi_type_pointer, dlopen, 2, type));

    type[0] = &ffi_type_pointer;
    put_cif(dlclose, lac_cif_alloc(&ffi_type_sint, dlclose, 1, type));

    type[0] = &ffi_type_pointer;
    type[1] = &ffi_type_pointer;
    put_cif(dlsym, lac_cif_alloc(&ffi_type_pointer, dlsym, 2, type));

#define X(A, B, C, D) put_ffi_type(B);
    FFI_TYPE_TABLE(X)
#undef X
    type[0] = &ffi_type_pointer;
    type[1] = &ffi_type_pointer;
    put_cif(parse, lac_cif_alloc(&ffi_type_variant, parse_, 2, type));

    type[0] = &ffi_type_variant;
    put_cif(print, lac_cif_alloc(&ffi_type_sint, print_, 1, type));

    put_cif(nl, lac_cif_alloc(&ffi_type_void, nl_, 0, NULL));
    put_cif(tab, lac_cif_alloc(&ffi_type_void, tab_, 0, NULL));

    type[0] = &ffi_type_pointer;
    put_cif(puts, lac_cif_alloc(&ffi_type_sint, puts, 1, type));

    put_cif(_, lac_cif_alloc(&ffi_type_void, print_map, 0, NULL));

    put_cif(getchar, lac_cif_alloc(&ffi_type_sint, getchar, 0, NULL));

    type[0] = &ffi_type_sint;
    put_cif(putchar, lac_cif_alloc(&ffi_type_sint, putchar, 1, type));

    // control flow
    /*
       type[0] = &ffi_type_variant;
       type[1] = &ffi_type_variant;
       put_cif(if, lac_cif_alloc(&ffi_type_variant, if_, 2, type));
     */
    type[0] = &ffi_type_pointer;
    type[1] = &ffi_type_pointer;
    put_cif(while, lac_cif_alloc(&ffi_type_void, while_, 2, type));
}

void lac_fini(void)
{
}
