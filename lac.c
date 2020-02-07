// lac.c - load and call C functions
#include <ctype.h>
#include <dlfcn.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gdbm.h>
#include "ensure.h"
#include "lac.h"

#define MAX_BUF 1024

typedef struct {
	char* b;
	char* e;
} token;

int line = 0;
const char* file = "";

lac_dbm dictionary;
lac_dbm library;
lac_stack* stack; // == macro

lac_datum make_datum(token_view t)
{
	return (lac_datum){(char*)t.b, t.e - t.b};
}

void lac_thunk_call(FILE* fp, lac_cif* thunk)
{
	// if not varargs
	ffi_cif* cif = &thunk->cif;
	size_t n = cif->nargs;
	lac_variant args[n];
	void* addr[n];

	for (size_t i = 0; i < n; ++n) {
		token_view arg = token_view_next(fp);
		ensure (!token_view_error(arg));
		if (*arg.b == ';' && arg.e - arg.b == 1) {
			for (; i < n; ++i) {
				ensure (lac_stack_count(stack) > 0);
				args[i] = *(lac_variant*)lac_stack_top(stack);
				addr[i] = lac_variant_address(&args[i]);
				lac_stack_pop(stack);
			}
		}
		else {
			lac_datum d = lac_dbm_fetch(dictionary, make_datum(arg));
			if (0 != d.data) {
				lac_thunk_call(fp, (lac_cif*)d.data);
			}
			else {
				args[i] = lac_variant_parse(cif->arg_types[i], arg.b, arg.e);
				addr[i] = lac_variant_address(&args[i]);
			}
		}
	}

	lac_cif_call(thunk, addr);
	if (cif->rtype != &ffi_type_void) {
		lac_stack_push(stack, &thunk->result);
	}
}

void lac_execute(FILE* fp)
{
	token_view v = token_view_next(fp);

	if (token_view_error(v)) {
		perror(v.b);

		return;
	}

	lac_datum datum = lac_dbm_fetch(dictionary, make_datum(v));
	if (0 == datum.data) {
		perror("lac_execute: thunk not found");

		return;
	}

	lac_cif* thunk = (lac_cif*)datum.data;
	lac_thunk_call(fp, thunk);

	if (token_view_last(v)) {
		return;
	}

	lac_execute(fp);
}

// add some thunks to the dictionary
void lac_init()
{
	{ // puts
		//lac_cif* = lac_cif_alloc(1, 
	}
}

int main(int ac, const char* av[])
{
	FILE* fp;
	// process args
	fp = ac > 1 ? fopen(av[1], "r") : stdin;
	ensure (fp);
	file = ac > 1 ? av[1] : "stdin";

	dictionary = lac_dbm_open("dictionary");
	library = lac_dbm_open("library");
	stack = LAC_STACK_ALLOC(1024, lac_variant);

	lac_init();

	// setjmp/longjmp for error handling
	//evaluate(fp);
	token_view v;
	do {
		v = token_view_next(fp);
		if (token_view_error(v)) {
			fputs("token_view_error: ", stdout);	
			v.e = 0;
			puts(v.b);
		}
		fputs(">", stdout);
		fwrite(v.b, 1, v.e - v.b, stdout); 
		fputs("<\n", stdout);
	} while (!token_view_last(v));
	//lac_execute(fp);

	LAC_STACK_FREE(stack);
	lac_dbm_close(library);
	lac_dbm_close(dictionary);

	return 0;
}
