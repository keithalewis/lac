// lac.c - load and call C functions
//#include <sys/queue.h>
#include <dlfcn.h>
#include <stdarg.h>
#include "ensure.h"
#include "lac.h"

#define MAX_BUF 1024

int line = 0;
const char* file = "";

lac_stack* stack;

void lac_call_thunk(lac_stream* fp, lac_variant* result, const lac_cif* thunk);

lac_variant lac_parse_token(lac_stream* fp, lac_token t, ffi_type* type)
{
	lac_variant v;

	const lac_cif* cif = lac_map_get(t);
	if (0 != cif) {
		lac_call_thunk(fp, &v, cif);
	}
	else {
		v = lac_variant_parse(type, t.b, t.e);
	}

	return v;
}

void lac_call_thunk(lac_stream* fp, lac_variant* result, const lac_cif* thunk)
{
	const ffi_cif* cif = &thunk->cif;
	size_t n = cif->nargs;
	lac_variant args[n];
	void* addr[n];

	for (size_t i = 0; i < n; ++i) {
		lac_token arg = lac_stream_token_next(fp);
		ensure (!lac_token_error(arg));
		if (';' == *arg.b && 1 == arg.e - arg.b) {
			// pop from stack
			while (i < n) {
				lac_variant* t = lac_stack_top(stack);
				ensure (0 != t);
				args[i] = *t;
				addr[i] = lac_variant_address(&args[i]);
				lac_stack_pop(stack);
				++i;
			}
		}
		else {
			*arg.e = 0;
			args[i] = lac_parse_token(fp, arg, cif->arg_types[i]);
			addr[i] = lac_variant_address(&args[i]);
		}
	}

	lac_cif_call(thunk, result, addr);
	// free pointer args???
}

// lookup and call thunk of token
int lac_execute_token(lac_stream* fp, lac_variant* result, lac_token t)
{
	// if not varargs
	ensure (!lac_token_error(t)); 

	const lac_cif* thunk = lac_map_get(t);
	ensure (thunk);

	lac_call_thunk(fp, result, thunk);

	return !lac_token_last(t);
}

void lac_execute(lac_stream* fp)
{
	lac_token t = lac_stream_token_next(fp);

	ensure (!lac_token_error(t));

	if (lac_token_last(t)) {
		return;
	}

	lac_variant result;
	lac_execute_token(fp, &result, t);
	if (result.type != &ffi_type_void) {
		lac_stack_push(stack, &result);
	}

	lac_execute(fp);
}

// terminate with null pointer
static void load(const char* lib, ffi_type* ret, const char* sym, ...)
{
	va_list ap;
	ffi_type* arg[32]; // maximum number of arguments

	va_start(ap, sym);
	unsigned n = 0;
	while (n < 32) {
		ffi_type* type = va_arg(ap, ffi_type*);
		if (!type) {
			break;
		}
		arg[n] = type;
		++n;
	}
	ensure (n < 32);
	va_end(ap);

	void* l = dlopen(lib, RTLD_LAZY);
	void* s = dlsym(l, sym);
	ensure (s);

	lac_cif* cif = lac_cif_alloc(ret, s, n, arg);
	size_t size = strlen(sym);
	char* t = malloc(size + 1);
	strcpy(t, sym);
	lac_map_put(LAC_TOKEN(t, t + size), cif);
}

static lac_variant pick(size_t n)
{
	return LAC_STACK_PICK(stack, n, lac_variant);
}

// add some thunks to the dictionary
void lac_init()
{
	{ // puts
		load("libc.so.6", &ffi_type_sint, "puts", &ffi_type_pointer, 0);
	}
	{
		lac_token key = LAC_TOKEN("type", 0);
		ffi_type* args[1] = {&ffi_type_pointer};
		lac_cif* cif = lac_cif_alloc(&ffi_type_pointer, ffi_type_lookup, 1, args);
		lac_map_put(key, cif);
	}
	{
		//static char* key = "pick";
		/*
		ffi_type* args[1] = {&ffi_type_pointer};
		lac_cif* cif = lac_cif_alloc(&ffi_type_pointer, ffi_type_lookup, 1, args);
		lac_map_put(LAC_TOKEN(key, key + 4), cif);
		*/
	}
	// del - delete dictionary entry
}

int main(int ac, const char* av[])
{
	FILE* fp;
	// process args
	fp = ac > 1 ? fopen(av[1], "r") : stdin;
	ensure (fp);
	file = ac > 1 ? av[1] : "stdin";

	stack = LAC_STACK_ALLOC(1024, lac_variant);

	lac_init();

	// setjmp/longjmp for error handling
	//evaluate(fp);
	lac_stream s = lac_stream_file(fp);
	/*
	do {
		lac_token v;
		v = lac_stream_token_next(&s);
		if (lac_token_error(v)) {
			fputs("lac_token_error: ", stdout);	
			v.e = 0;
			puts(v.b);

			continue;
		}
		fputs(">", stdout);
		fwrite(v.b, 1, v.e - v.b, stdout); 
		fputs("<\n", stdout);
	} while (!lac_token_last(v));
	*/
	lac_execute(&s);

	LAC_STACK_FREE(stack);

	return 0;
}
