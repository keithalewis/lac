// lac_ffi.c
#include "lac_ffi.h"

#define X(A,B,C,D) lac_variant lac_variant_parse_##D(const char* b, const char* e) \
{	return lac_variant_parse(C, b, e); }
	FFI_TYPE_TABLE(X)
#undef X

inline void* lac_variant_address(lac_variant* pv)
{
#define X(a,b,c,d) if (c == pv->type) return &pv->value.d;	
	FFI_TYPE_TABLE(X)
#undef X
	return 0;
}

// int lac_parse_i(const char* b, const char* e)
// double lac_parse_d(const char* b, const char* e)
// ...
// 
// Array indexed by FFI_TYPE_XXX
// static void* lac_parse_table[] = { &lac_parse_i, ... }

lac_variant lac_variant_parse(ffi_type* type, const char* b, const char* e)
{
	lac_variant v;

	v.type = type;
	if (&ffi_type_sint == type) {
		char* e_;
		// check size!!!
		v.value.i = (int)strtol(b, &e_, 0);
		assert (e == e_);
	}
	else if (&ffi_type_uint32 == type) {
		char* e_;
		// check size!!!
		v.value.u32 = (uint32_t)strtoul(b, &e_, 0);
		assert (e == e_);
	}
	else if (&ffi_type_float == type) {
		char* e_;
		v.value.f = strtof(b, &e_);
		assert (e == e_);
	}
	else if (&ffi_type_double == type) {
		char* e_;
		v.value.d = strtod(b, &e_);
		assert (e == e_);
	}
	// etc!!!

	return v;
}

size_t lac_cif_size(lac_cif* pcif)
{
	return sizeof(lac_cif) + pcif->cif.nargs*sizeof(void*);
}

// Allocate and set nargs
lac_cif* lac_cif_alloc(int n)
{
	lac_cif* p = (lac_cif*)malloc(sizeof(lac_cif) + n*sizeof(void*));
	p->cif.nargs = n;

	return p;
}
lac_cif* lac_cif_realloc(lac_cif* p, int n)
{
	p = (lac_cif*)realloc(p, sizeof(lac_cif) + n*sizeof(void*));
	p->cif.nargs = n;

	return p;
}

void lac_cif_free(lac_cif* p)
{
	free(p);
}

ffi_status lac_cif_prep(lac_cif* pcif, ffi_type* rtype, ffi_type**arg_types)
{
	pcif->cif.arg_types = &pcif->arg_types[0];
	memcpy(pcif->arg_types, arg_types, pcif->cif.nargs*sizeof(void*));

	return ffi_prep_cif(&pcif->cif, FFI_DEFAULT_ABI, pcif->cif.nargs, rtype, pcif->arg_types);
}

ffi_status lac_cif_prep_var(lac_cif* pcif, unsigned nargs, ffi_type**arg_types)
{
	unsigned nfix = pcif->cif.nargs; // number of fixed args
	void* types[nfix + nargs];
	memcpy(types, pcif->cif.arg_types, pcif->cif.nargs*sizeof(void*));

	return ffi_prep_cif_var(&pcif->cif, FFI_DEFAULT_ABI, nfix, nfix + nargs, pcif->cif.rtype, &types);
}

void lac_cif_call(lac_cif* pcif, ffi_arg* ret, void** args)
{
	ffi_call(&pcif->cif, pcif->sym, ret, args);
}

lac_variant* lac_stack_top(lac_stack* stack)
{
	return stack->data + stack->sp;
}
void* lac_stack_address(lac_stack* stack)
{
	return stack->addr + stack->sp;
}
size_t lac_stack_size(lac_stack* stack)
{
	return stack->sp - STACK_SIZE;
}

void lac_stack_push(lac_stack* stack, lac_variant v)
{
	stack->data[stack->sp] = v;
	stack->addr[stack->sp] = lac_variant_address(stack->data + stack->sp);
	--stack->sp;
}
void lac_stack_pop(lac_stack* stack)
{
	if (stack->sp < STACK_SIZE - 1) {
		++stack->sp;
	}
}

lac_variant* lac_stack_pull(lac_stack* stack, size_t n)
{
	assert (n < lac_stack_size(stack));

	lac_stack_push(stack, stack->data[stack->sp + n - 1]);

	return lac_stack_top(stack);
}

lac_variant* lac_stack_roll(lac_stack* stack, size_t n)
{
	lac_variant v = stack->data[stack->sp + n - 1];
	void* p = stack->addr[stack->sp + n - 1];

	memmove(stack->data + 1, stack->data, (n - 1)*sizeof(lac_variant));
	memmove(stack->addr + 1, stack->addr, (n - 1)*sizeof(void*));

	stack->data[stack->sp] = v;
	stack->addr[stack->sp] = p;

	return lac_stack_top(stack);
}
