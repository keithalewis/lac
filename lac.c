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

lac_variant lac_call_thunk(FILE * fp, const lac_cif * thunk);

// figure out what kind of token and ensure correct type
lac_variant lac_evaluate_token(FILE* fp, ffi_type* type, char* token)
{
	ensure (token);

	lac_variant v = { .type = &ffi_type_void };

	if (*token == '"' || *token == '"') {
		v.type = &ffi_type_pointer;
		v.value._pointer = token;
		// don't free token
	}
	else {
		const lac_cif* thunk = lac_map_get(token);
		if (thunk) {
			v = lac_call_thunk(fp, thunk);
		}
		else {
			FILE* is = fmemopen(token, strlen(token), "r");
			v.type = type;
			ensure (0 < lac_variant_scan(is, &v));
			fclose(is);
		}

		free(token);
	}

	ensure (v.type == type);

	return v;
}

// call thunk on arguments from input stream
lac_variant lac_call_thunk(FILE * fp, const lac_cif * thunk)
{
	ensure (thunk);

	lac_variant v = { .type = &ffi_type_void };
	const ffi_cif *cif = &thunk->cif;
	int n = cif->nargs;

	if (n == 0) {
		lac_cif_call(thunk, &v, NULL);
	}
	else if (n > 0) {
		// VLAs that get cleaned up after the call
		lac_variant args[n];
		void* addr[n];

		for (int i = 0; i < n; ++i) {
			char* token = lac_parse_token(fp);
			args[i] = lac_evaluate_token(fp, cif->arg_types[i], token);
			addr[i] = lac_variant_address(&args[i]);
		}

		lac_cif_call(thunk, &v, addr);

		for (int i = 0; i < n; ++i) {
			lac_variant_free(&args[i]);
		}
	}
	/*
	else {		// varargs
	}
	*/
	// free pointer args???

	return v;
}

// call the thunk corresponding to token
lac_variant lac_call_token(FILE* fp, char* token)
{
	lac_variant v = { .type = &ffi_type_void };

	const lac_cif* thunk = lac_map_get(token);
	ensure (thunk);
	v = lac_call_thunk(fp, thunk);

	return v;
}

// parse tokens and call their thunk
lac_variant lac_execute(FILE* fp)
{
	lac_variant v = { .type = &ffi_type_void };

	char* token;
	while ((token = lac_parse_token(fp))) {
		v = lac_call_token(fp, token);
		free(token);
	}

	return v;
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
	lac_variant v = lac_execute(fp);

	return v.type == &ffi_type_sint ? v.value._sint : 0;
}
