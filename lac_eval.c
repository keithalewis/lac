// lac_eval.c
#define _GNU_SOURCE
#include <stdio.h>
#include "ensure.h"
#include "lac_parse.h"
#include "lac_map.h"
#include "lac_cif.h"
#include "lac_eval.h"

static lac_variant lac_token(FILE * fp)
{
	lac_variant v = {.type = &ffi_type_string_malloc };

	size_t n;
	v.value._pointer = lac_token_parse(fp, &n);
	ensure(n != (size_t) EOF);	// pass pointer to ensure???
	// free pointer ???

	return v;
}

// Call C function
static lac_variant lac_call(FILE * fp, lac_cif * cif);

// read stream and convert to type
lac_variant lac_eval(FILE * fp, ffi_type * type)
{
	lac_variant result;

	lac_variant token = lac_token(fp);

	if (token.type == type) {	// malloc'd ???
		return token;	// must be free'd
	}

	/*
	   if (token.value._pointer[0] == '`'') {
	   // ??? string_malloc
	   // don't lookup
	   return lac_variant_parse(type, token.value._pointer + 1);
	   }
	 */
	const lac_variant *pv = lac_map_get(token.value._pointer);
	if (pv) {		// in dictionary
		if (pv->type == &ffi_type_cif) {
			lac_cif *cif = pv->value._pointer;
			result = lac_call(fp, cif);
		} else {
			result = *pv;	// must not be free'd!!!
		}
	} else {
		result = lac_variant_parse(type, token.value._pointer);
		//lac_variant_free(&token);
	}
	//else if (type == &ffi_type_string) {
	//ensure (0 <= lac_variant_scan(fp, &result));
	//}

	return result;
}

lac_variant lac_call(FILE * fp, lac_cif * cif)
{
	ffi_cif *ffi = &cif->cif;
	int n = (int)ffi->nargs;
	lac_variant v;
	v.type = ffi->rtype;
	// allocate memory for v based on v.type???

	if (n == 0) {
		ffi_call(ffi, cif->sym, lac_variant_address(&v), NULL);
	} else if (n > 0) {
		lac_variant args[n];
		void *addr[n];

		for (int i = 0; i < n; ++i) {
			args[i] = lac_eval(fp, ffi->arg_types[i]);
			addr[i] = lac_variant_address(&args[i]);
		}

		ffi_call(ffi, cif->sym, lac_variant_address(&v), addr);

		for (int i = 0; i < n; ++i) {
			lac_variant_free(&args[i]);
		}
	} else {
		ensure(n < 0);
		// variadic
	}

	return v;
}

///////////////////////

// read token from stream and convert to type
lac_variant lac_eval_type(FILE * fp, ffi_type * type)
{
	lac_variant token = lac_token(fp);

	if (type == &ffi_type_string || type == &ffi_type_string_malloc) {
		return token;	// returns type &ffi_type_string_malloc
	}

	const lac_variant *pv = lac_map_get(token.value._pointer);

	lac_variant result;
	if (pv) {		// in dictionary
		if (pv->type == &ffi_type_cif) {
			//ensure (type == &ffi_type_cif || type == &ffi_type_cif_malloc);
			lac_cif *cif = pv->value._pointer;
			result = lac_call_cif(fp, cif);
		} else {
			result = *pv;	// must not be free'd!!!
			//ensure (pv->type == type);
		}
	} else {
		// ensure type is scalar???
		result = lac_variant_parse(type, token.value._pointer);
	}

	return result;
}

// read arguments from stream and call cif
lac_variant lac_call_cif(FILE * fp, lac_cif * cif)
{
	ffi_cif *ffi = &cif->cif;
	int n = (int)ffi->nargs;
	lac_variant result;
	result.type = ffi->rtype;
	// allocate memory for result based on result.type???

	if (n == 0) {
		ffi_call(ffi, cif->sym, lac_variant_address(&result), NULL);
	} else if (n > 0) {
		lac_variant args[n];
		void *addr[n];

		for (int i = 0; i < n; ++i) {
			args[i] = lac_eval_type(fp, ffi->arg_types[i]);
			addr[i] = lac_variant_address(&args[i]);
		}

		ffi_call(ffi, cif->sym, lac_variant_address(&result), addr);

		for (int i = 0; i < n; ++i) {
			/*
			   if (args[i].type == &ffi_pointer_malloc) {
			   free (args[i].value._pointer);
			   }
			 */
		}
	} else {
		ensure(n < 0);
		// variadic
	}

	return result;
}
