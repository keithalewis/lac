// lac_ffi.c
#include <dlfcn.h>
#include <stdarg.h>
#include "ensure.h"
#include "lac_cif.h"

size_t lac_cif_size(lac_cif * pcif)
{
    return sizeof(lac_cif) + pcif->cif.nargs * sizeof(void *);
}

// Allocate and set nargs
lac_cif *lac_cif_alloc(ffi_type * rtype, void *sym,
		       unsigned nargs, ffi_type ** arg_types)
{
    lac_cif *p = malloc(sizeof(lac_cif) + nargs * sizeof(void *));
    ensure(0 != p);

    p->cif.rtype = rtype;
    p->sym = sym;
    p->cif.nargs = nargs;
    p->cif.arg_types = &p->arg_types[0];
    memcpy(p->cif.arg_types, arg_types, nargs * sizeof(ffi_type *));

    lac_cif_prep(p);

    return p;
}

ffi_status lac_cif_prep(lac_cif * pcif)
{
    return ffi_prep_cif(&pcif->cif, FFI_DEFAULT_ABI,
			pcif->cif.nargs, pcif->cif.rtype, pcif->cif.arg_types);
}

// prepare additional args for vararg functions
lac_cif *lac_cif_prep_var(const lac_cif * p, unsigned nargs,
			  ffi_type ** arg_types)
{
    unsigned nfix = -p->cif.nargs;	// number of fixed args
    lac_cif *p_ = malloc(sizeof(lac_cif) + (nfix + nargs) * sizeof(ffi_type *));
    ensure(0 != p_);

    p_->cif.rtype = p->cif.rtype;
    p_->sym = p->sym;
    p_->cif.nargs = p->cif.nargs + nargs;
    p_->cif.arg_types = &p_->arg_types[0];
    memcpy(p_->cif.arg_types, p->cif.arg_types, nfix * sizeof(ffi_type *));
    memcpy(p_->cif.arg_types + nfix, arg_types, nargs * sizeof(ffi_type *));

    ffi_status ret = ffi_prep_cif_var(&p_->cif, FFI_DEFAULT_ABI, nfix,
				      nfix + nargs, p_->cif.rtype,
				      p_->cif.arg_types);
    ensure(FFI_OK == ret);

    return p_;
}

void lac_cif_call(lac_cif * pcif, lac_variant * result, void **args)
{
    result->type = pcif->cif.rtype;

    ffi_call(&pcif->cif, FFI_FN(pcif->sym), lac_variant_address(result), args);
}

static lac_cif *lac_cif_vload(const char *lib, ffi_type * ret,
			      const char *sym, va_list ap)
{
    ffi_type *arg[32];		// maximum number of arguments

    unsigned n = 0;
    while (n < 32) {
	ffi_type *type = va_arg(ap, ffi_type *);
	if (!type) {
	    break;
	}
	arg[n] = type;
	++n;
    }
    ensure(n < 32);

    void *l = dlopen(lib, RTLD_LAZY);
    void *s = dlsym(l, sym);
    ensure(s);

    return lac_cif_alloc(ret, s, n, arg);
}

lac_cif *lac_cif_load(const char *lib, ffi_type * ret, const char *sym, ...)
{
    lac_cif *cif;
    va_list ap;

    va_start(ap, sym);
    cif = lac_cif_vload(lib, ret, sym, ap);
    va_end(ap);

    return cif;
}

lac_cif *lac_cif_loadv(const char *lib, ffi_type * ret, const char *sym, ...)
{
    lac_cif *cif;
    va_list ap;

    va_start(ap, sym);
    cif = lac_cif_vload(lib, ret, sym, ap);
    va_end(ap);

    cif->cif.nargs = -cif->cif.nargs;	// indicates varargs

    return cif;
}
