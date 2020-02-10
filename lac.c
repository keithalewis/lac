// lac.c - load and call C functions
//#include <sys/queue.h>
#include "ensure.h"
#include "lac.h"

#define MAX_BUF 1024

int line = 0;
const char* file = "";

lac_dbm dictionary;
lac_dbm library;
lac_stack* stack;

lac_datum make_datum(token_view t)
{
	return (lac_datum){(char*)t.b, t.e - t.b};
}

void lac_call_thunk(FILE* fp, lac_variant* result, lac_cif* thunk);

lac_variant lac_parse_token(FILE* fp, token_view t, ffi_type* type)
{
	lac_variant v;

	lac_datum datum = lac_dbm_fetch(dictionary, make_datum(t));
	if (0 != datum.data) {
		lac_call_thunk(fp, &v, (lac_cif*)datum.data);
	}
	else {
		v = lac_variant_parse(type, t.b, t.e);
	}

	return v;
}

void lac_call_thunk(FILE* fp, lac_variant* result, lac_cif* thunk)
{
	ffi_cif* cif = &thunk->cif;
	size_t n = cif->nargs;
	lac_variant args[n];
	void* addr[n];

	for (size_t i = 0; i < n; ++n) {
		token_view arg = token_view_next(fp);
		ensure (!token_view_error(arg));
		args[i] = lac_parse_token(fp, arg, cif->arg_types[i]);
		// allocate all pointer types
		addr[i] = lac_variant_address(&args[i]);
	}

	lac_cif_call(thunk, result, addr);
	// free pointer args???
}

// lookup and call thunk of token
int lac_execute_token(FILE* fp, lac_variant* result, token_view v)
{
	// if not varargs
	ensure (!token_view_error(v)); 

	lac_datum datum = lac_dbm_fetch(dictionary, make_datum(v));
	ensure (0 != datum.data);

	lac_cif* thunk = (lac_cif*)datum.data;
	lac_call_thunk(fp, result, thunk);

	return !token_view_last(v);
}

void lac_execute(FILE* fp)
{
	token_view t = token_view_next(fp);

	ensure (!token_view_error(t));

	lac_variant result;
	while (lac_execute_token(fp, &result, t)) {
		if (result.type != &ffi_type_void) {
			lac_stack_push(stack, &result);
		}
	}

	if (!token_view_last(t)) {
		lac_execute(fp);
	}
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

			continue;
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
