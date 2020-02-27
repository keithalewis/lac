// lac.c - load and call C functions
//#include <sys/queue.h>
#include <dlfcn.h>
#include <stdarg.h>
#include "ensure.h"
#include "lac.h"

int line = 0;
const char *file = "";

jmp_buf lac_jmp_buf;
const char* lac_strerror = "";

lac_variant lac_evaluate(FILE* fp);

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
	
	if (token.type == &ffi_type_block) {
		result = lac_evaluate_block(token);
		lac_variant_free(&token);
	}
	else {
		ensure (token.type == &ffi_type_string);
		const lac_variant* pv = lac_map_get(token.value._pointer);
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

int main(int ac, const char *av[])
{
	// atexit??? on_exit???
	FILE *fp;
	//!!! process args to allow multiple input files
	fp = ac > 1 ? fopen(av[1], "r") : stdin;
	ensure(fp);
	file = ac > 1 ? av[1] : "stdin";

	lac_init();

	if (setjmp(lac_jmp_buf)) {
		fputs(lac_strerror, stderr);
		lac_strerror = "";
	}

	// for file ... lac_execute
	lac_variant result;
	while (!feof(fp)) {
		result = lac_evaluate(fp);
		lac_variant_free(&result);
	}

	return result.type == &ffi_type_sint ? result.value._sint : 0;
}
