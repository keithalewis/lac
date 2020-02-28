// lac_eval.c
#include <stdio.h>
#include "lac_map.h"
#include "lac_eval.h"

// evaluate token to type
/*
const lac_variant lac_eval(ffi_type* type, const lac_variant token)
{
	if (type == token.type || type == &ffi_type_variant) {
		return token;
	}

	if (token.type == &ffi_type_string) {
		//const lac_variant* pv = lac_map_get(token.value._pointer);
		const lac_variant* pv = lac_map_get(lac_variant_address(token);
		if (pv) { // in dictionary
			if (pv->type == &ffi_type_cif) {
				lac_cif* cif = pv->value._pointer;
				result = lac_evaluate_cif(cif, fp);
			}
			else {
				result = *pv;
			}
		}
		else {
			result = token;
			// don't free token
		}
	}
}
*/
lac_variant lac_evaluate_type(ffi_type* type, const lac_variant token)
{
	ensure (token.type == &ffi_type_string);
	
	return lac_variant_parse(type, token.value._pointer);
}

lac_variant lac_evaluate_cif(lac_cif* cif, FILE* fp)
{
	ffi_cif* ffi = &cif->cif;
	int n = ffi->nargs;
	lac_variant v;
	v.type = ffi->rtype;

	if (n == 0) {
		ffi_call(ffi, cif->sym, lac_variant_address(&v), NULL);
	}
	else if (n > 0) {
		lac_variant args[n];
		void* addr[n];

		for (int i = 0; i < n; ++i) {
			args[i] = lac_evaluate(fp);
			if (args[i].type != ffi->arg_types[i]) {
				lac_variant ai = lac_evaluate_type(ffi->arg_types[i], args[i]);
				lac_variant_free(&args[i]);
				args[i] = ai;
			}
			addr[i] = lac_variant_address(&args[i]);
		}

		ffi_call(ffi, cif->sym, lac_variant_address(&v), addr);

		for (int i = 0; i < n; ++i) {
			lac_variant_free(&args[i]);
		}
	}
	else {
		// variadic
	}

	return v;
}

lac_variant lac_evaluate_block(const lac_variant token)
{
	ensure (token.type == &ffi_type_block);
	lac_variant result = { .type = &ffi_type_void };

	FILE *fmemopen(void *buf, size_t size, const char *mode);
	FILE* is = fmemopen(token.value._pointer,
				strlen(token.value._pointer), "r");
	result = lac_evaluate(is);
	fclose (is);

	return result;
}

// parse tokens and call their cif
// result needs to be free'd
lac_variant lac_evaluate(FILE* fp)
{
	lac_variant result = { .type = &ffi_type_void };

	lac_variant token = lac_parse_token(fp);

	if (token.type == &ffi_type_void) {
		return token;
	}
	
	if (token.type == &ffi_type_block) {
		result = lac_evaluate_block(token);
		lac_variant_free(&token);
	}
	else {
		ensure (token.type == &ffi_type_string);
		//const lac_variant* pv = lac_map_get(token.value._pointer);
		const lac_variant* pv = lac_map_get(lac_variant_address(&token));
		if (pv) {
			if (pv->type == &ffi_type_cif) {
				lac_cif* cif = pv->value._pointer;
				result = lac_evaluate_cif(cif, fp);
			}
			else {
				result = *pv;
			}
			lac_variant_free(&token);
		}
		else {
			result = token;
			// don't free token
		}
	}

	return result;
}
