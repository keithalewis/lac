// lac_eval.c
#define _GNU_SOURCE
#include <stdio.h>
#include "lac_map.h"
#include "lac_eval.h"

// read stream and convert to type
lac_variant lac_eval(FILE* fp, ffi_type* type)
{
	lac_variant result;

	lac_variant token = lac_token_parse(fp);

	if (token.type == type) { // || type == &ffi_type_variant) {
		return token; // must be free'd
	}
	
	if (token.type == &ffi_type_string) {
		const lac_variant* pv = lac_map_get(token.value._pointer);
		if (pv) { // in dictionary
			if (pv->type == &ffi_type_cif) {
				lac_cif* cif = pv->value._pointer;
				result = lac_eval_cif(fp, cif);
			}
			else {
				result = *pv; // must not be free'd!!!
			}
		}
		else {
			result = lac_variant_parse(type, token.value._pointer);
		}
	}
	//else if (type == &ffi_type_string) {
		//ensure (0 <= lac_variant_scan(fp, &result));
	//}

	return result;
}

lac_variant lac_eval_cif(FILE* fp, lac_cif* cif)
{
	ffi_cif* ffi = &cif->cif;
	int n = ffi->nargs;
	lac_variant v;
	v.type = ffi->rtype;
	// allocate memory for v based on v.type???

	if (n == 0) {
		ffi_call(ffi, cif->sym, lac_variant_address(&v), NULL);
	}
	else if (n > 0) {
		lac_variant args[n];
		void* addr[n];

		for (int i = 0; i < n; ++i) {
			args[i] = lac_eval(fp, ffi->arg_types[i]);
			addr[i] = lac_variant_address(&args[i]);
		}

		ffi_call(ffi, cif->sym, lac_variant_address(&v), addr);

		for (int i = 0; i < n; ++i) {
	//!!!		lac_variant_free(&args[i]);
		}
	}
	else {
		// variadic
	}

	return v;
}

lac_variant lac_evaluate_block(const lac_variant token)
{
	lac_variant result = { .type = &ffi_type_void };

	FILE *fmemopen(void *buf, size_t size, const char *mode);
	FILE* is = fmemopen(token.value._pointer,
				strlen(token.value._pointer), "r");
	//result = lac_evaluate(is);
	fclose (is);

	return result;
}
