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

// return first character after white or not space
static int skip(FILE* fp, int(*is)(int), int space)
{
	int c = fgetc(fp);

	while (EOF != c && space ? is(c) : !is(c)) {
		c = fgetc(fp);
	}

	return c;
}
// return EOF or e
/*
static int skip_to(FILE* fp, int e)
{
	int c = fgetc(fp);

	while (EOF != c && e != c) {
		c = fgetc(fp);
	}
	
	return c;
}
static token_view next_token(FILE* fp)
{
	static char buf[MAX_BUF];

	const char* b = buf;
	const char* e = buf + MAX_BUF;

	//int c = skip(fp, isspace, true);
	// switch c...

	return (token_view){b,e};
}
*/

lac_datum make_datum(token_view t)
{
	return (lac_datum){(char*)t.b, t.e - t.b};
}

int int_(const char* s)
{
	return lac_parse_i(s, s + strlen(s));
}
double double_(const char* s)
{
	return lac_parse_d(s, s + strlen(s));
}
void* pointer_(const char* s)
{
	return (void*)s;
}

void evaluate_line(FILE* fp)
{
	//token t = token_view_next(fp);
	// look up token in dictionary
	// get arguments based on cif
	// call
}

void add_dictionary(FILE* fp)
{
	// get name
	// compile
	// add to dictionary
}

/* should not special case this
void load_library(FILE* fp)
{
	int c;
	token_view t;

	c = fgetc(fp);
	ensure (c == 'l');
	
	void* lib; // = load_lib(fp);

	// next token is return type
	t = next_token(fp);
	ffi_type* rtype = ffi_type_lookup(t.b);
	ensure (0 != rtype);

	// symbol
	t = next_token(fp);
	const char* symbol = t.b;
	*t.e = 0;
	void* sym = dlsym(lib, symbol);
	ensure (0 != sym);
	
	// arg types
	ffi_type* args[32];
	size_t n = 0;
	t = next_token(fp);
	while (t.b != t.e) {
		if (t.e - t.b == 3 && 0 == strcmp(t.b, "...")) { // varargs
			//!!! make sure no more args
			break;
		}
		args[n] = ffi_type_lookup(t.b);
		++n;
		if ('\n' == *t.e) {
			break;
		}
		t = next_token(fp);
	}

	lac_cif* pcif = lac_cif_alloc(n);
	ensure (0 != pcif);
	ensure (FFI_OK == lac_cif_prep(pcif, rtype, args));

	lac_datum key = { (char*)symbol, strlen(symbol) };
	lac_datum val = { (char*)pcif, lac_cif_size(pcif) };

	int ret = lac_dbm_insert(dictionary, key, val);
	lac_cif_free(pcif);
	//!!! check if duplicate entry
}
*/

void lac_execute(FILE* fp)
{
	int c = skip(fp, isspace, true);

	if (EOF == c) return;

	// next_token
	// lookup
	// call

	lac_execute(fp);
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

	// setjmp/longjmp for error handling
	//evaluate(fp);
	lac_execute(fp);

	LAC_STACK_FREE(stack);
	lac_dbm_close(library);
	lac_dbm_close(dictionary);

	return 0;
}
