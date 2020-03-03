/* lac_ffi.h - load and call C functions using libffi */
#pragma once
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <ffi.h>
#ifdef __cplusplus
}
#endif
#include "lac_variant.h"

// thunk corresponding to string name of symbol
typedef struct {
	void *sym;
	ffi_cif cif;
	ffi_type *arg_types[];	// flexible length array
} lac_cif;

// allocate n args and set cif.arg_types to arg_types
lac_cif *lac_cif_alloc(ffi_type * rtype, void *sym,
		       unsigned nargs, ffi_type ** arg_types);
void lac_cif_free(lac_cif * pcif);
size_t lac_cif_size(lac_cif * pcif);

// call ffi_prep_cif using cif->cif.nargs
ffi_status lac_cif_prep(lac_cif * pcif);

// call ffi_prep_cif_var for variadic functions
// where nargs is the number of variable args
// unlike ffi_prep_cif_var, arg_types only has nargs items
lac_cif *lac_cif_prep_var(const lac_cif * pcif, unsigned nargs,
			  ffi_type ** arg_types);

// call ffi_call and store result
void lac_cif_call(const lac_cif * pcif, lac_variant * result, void **args);

// load symbol from library
// terminate arg types with null pointer
lac_cif *lac_cif_load(const char *lib, ffi_type * ret, const char *sym, ...);
// load varargs function (nargs = -nargs)
lac_cif *lac_cif_loadv(const char *lib, ffi_type * ret, const char *sym, ...);
