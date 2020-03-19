// lac_eval.t.c - test evaluation
#include "ensure.h"
#include <stdio.h>
#include <string.h>
#include "lac_cif.h"
#include "lac_eval.h"

static inline FILE* is(char* s)
{
	return fmemopen(s, strlen(s), "r");
}

static inline FILE* os(char** s, size_t* n)
{
	return open_memstream(s, n);
}

static int test_lac_eval_cif()
{
	{
		char* s = "\"hello cif\"";
        lac_cif *pcif = lac_cif_alloc(&ffi_type_sint, puts, 1, (ffi_type*[]){&ffi_type_pointer});
		lac_variant v = lac_call_cif(is(s), pcif);
		ensure (v.type == &ffi_type_sint);
		ensure (v.value._sint = strlen(s) + 1);
		free(pcif);
	}
	/*
	{
		char ibuf[1024];
		char* obuf;
		size_t n;
		FILE* o = os(&obuf, &n);
		FILE* i = is(s);
        lac_cif *pcif = lac_cif_alloc(&ffi_type_sint, puts, 1, (ffi_type*[]){&ffi_type_pointer});
		lac_variant v = lac_call_cif(is(s), pcif);
		ensure (v.type == &ffi_type_sint);
		ensure (v.value._sint = strlen(s) + 1);
		free(pcif);
	}
	*/

	return 0;
}

int test_lac_eval();
extern int test_lac_eval()
{
	test_lac_eval_cif();

    return 0;
}
