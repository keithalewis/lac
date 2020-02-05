// lac.c - load and call C functions
#include <assert.h>
#include <ctype.h>
#include <dlfcn.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gdbm.h>
#include "lac.h"

#define ensure assert
// redefine to use setjmp/longjmp for error handling
#define MAX_BUF 1024

typedef struct {
	char* b;
	char* e;
} token;

int line = 0;
const char* file = "";

lac_dbm dictionary;
lac_dbm library;
LAC_STACK(stack);

// return first character after white space, or not
static int skip(FILE* fp, int(*sp)(int), int space)
{
	int c = fgetc(fp);

	while (EOF != c && space ? sp(c) : !sp(c)) {
		c = fgetc(fp);
	}

	return c;
}
// return EOF or e
static int skip_to(FILE* fp, int e)
{
	int c = fgetc(fp);

	while (EOF != c && e != c) {
		c = fgetc(fp);
	}
	
	return c;
}
static token next_token(FILE* fp)
{
	static char buf[MAX_BUF];
	token v;

	v.b = v.e = buf;

	int c = skip(fp, isspace, true);
	// switch c...
	while (EOF != c && !isspace(c)) {
		*v.e++ = c;
		ensure (v.e - v.b < MAX_BUF);
		c = fgetc(fp);
	}
	*v.e = c; // include last character

	return v;
}

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
	//token t = get_token(fp);
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

void* load_lib(FILE* fp)
{
	char lib[1024] = "lib";

	token t = next_token(fp);
	strncat(lib + 3, t.b, t.e - t.b);
	strcat(lib, ".so.6"); // .dll for Windows .6 for gnu v6?

	return dlopen(lib, RTLD_LAZY);
}

// "-lxxx" -> dlopen("libxxx.so")
void load_library(FILE* fp)
{
	int c;
	token t;

	c = fgetc(fp);
	ensure (c == 'l');
	
	void* lib = load_lib(fp);

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

void lac_execute(FILE* fp)
{
	int c = skip(fp, isspace, true);

	if (EOF == c) return;

	if (c == '#') {
		// skip to next newline
		c = skip_to(fp, '\n');
		if (EOF == c)  return;

		lac_execute(fp);
	}

	switch (c) {
		case '-':
			load_library(fp);
			break;
		//case ':':
		//default:
		//	lac_cif* thunk = lac_compile(fp);
	}

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

	// setjmp/longjmp for error handling
	//evaluate(fp);
	lac_execute(fp);

	lac_dbm_close(library);
	lac_dbm_close(dictionary);

	return 0;
}
