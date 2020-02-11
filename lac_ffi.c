// lac_ffi.c
#include "ensure.h"
#include "lac_ffi.h"

size_t lac_cif_size(lac_cif* pcif)
{
	return sizeof(lac_cif) + pcif->cif.nargs*sizeof(void*);
}

// Allocate and set nargs
lac_cif* lac_cif_alloc(ffi_type* rtype, void* sym,
	unsigned nargs, ffi_type **arg_types)
{
	lac_cif* p = malloc(sizeof(lac_cif) + nargs*sizeof(void*));
	ensure (0 != p);

	p->cif.rtype = rtype;
	p->sym = sym;
	p->cif.nargs = nargs;
	p->cif.arg_types = &p->arg_types[0];
	memcpy(p->cif.arg_types, arg_types, nargs*sizeof(ffi_type*));

	lac_cif_prep(p);

	return p;
}

void lac_cif_free(lac_cif* p)
{
	free(p);
}

ffi_status
lac_cif_prep(lac_cif* pcif)
{
	return ffi_prep_cif(&pcif->cif, FFI_DEFAULT_ABI,
	                    pcif->cif.nargs, pcif->cif.rtype, pcif->cif.arg_types);
}

// prepare additional args for vararg functions
lac_cif* lac_cif_prep_var(lac_cif* p, unsigned nargs, ffi_type** arg_types)
{
	unsigned nfix = p->cif.nargs; // number of fixed args
	lac_cif* p_ = malloc(sizeof(lac_cif) + (nfix + nargs)*sizeof(ffi_type*));
	ensure (0 != p_);

	p_->cif.rtype = p->cif.rtype;
	p_->sym = p->sym;
	p_->cif.nargs = p->cif.nargs + nargs;
	p_->cif.arg_types = &p_->arg_types[0];
	memcpy(p_->cif.arg_types, p->cif.arg_types, nfix*sizeof(ffi_type*));
	memcpy(p_->cif.arg_types + nfix, arg_types, nargs*sizeof(ffi_type*));

	ffi_status ret = ffi_prep_cif_var(&p_->cif, FFI_DEFAULT_ABI, nfix,
                   	 nfix + nargs, p_->cif.rtype, p_->cif.arg_types);
	ensure (FFI_OK == ret);

	return p_;
}

void lac_cif_call(const lac_cif* pcif, lac_variant* result, void** args)
{
	result->type = pcif->cif.rtype;

	ffi_call((ffi_cif*)&pcif->cif, pcif->sym, lac_variant_address(result), args);
}
