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
		if (v.type == &ffi_type_pointer) {
			// string
			unsigned n = lac_token_size(t);
			char* ps = malloc(n + 1);
			strncpy(ps, v.value.p, n);
			v.value.p = ps;
		}
	}

	return v;
}

// fill args and addr with arguments given the type
void lac_parse_args(lac_stream* fp, unsigned n, ffi_type** types,
	lac_variant* args, void** addr)
{
	for (unsigned i = 0; i < n; ++i) {
		lac_token arg = lac_stream_token_next(fp);
		ensure (!lac_token_error(arg));
		ensure (!lac_token_last(arg));
		args[i] = lac_parse_token(fp, arg, types[i]);
		addr[i] = lac_variant_address(&args[i]);
	}
}

void lac_call_thunk(lac_stream* fp, lac_variant* result, const lac_cif* thunk)
{
	const ffi_cif* cif = &thunk->cif;
	int n = cif->nargs;

	if (n == 0) {
		lac_cif_call(thunk, result, NULL);
	}
	else if (n > 0) {
		//??? local stack for string args???
		lac_variant args[n];
		void* addr[n];
		lac_parse_args(fp, n, cif->arg_types, args, addr);
		lac_cif_call(thunk, result, addr);
	}
	/*
	else { // varargs
		lac_variant args[32];
		void* addr[32];
		n = -n; // fixed args
		// get fixed arguments
		lac_parse_args(fp, n, cif->arg_types, args, addr);
		//??? what are the types???
		//lac_token arg = lac_stream_token_next(fp);
		ffi_type* types[32];
		lac_token arg;
		unsigned i = 0;
		do {
			ensure (i < 32);
			arg = lac_stream_token_next(fp);
			ensure (!lac_token_error(arg));
			ensure (!lac_token_last(arg));
			args[i] = lac_parse_token(fp, arg, cif->arg_types[i]);
			addr[i] = lac_variant_address(&args[i]);
			types[i - n] = args[i].type;
			++i;
		} while (*arg.e != '\n');

		lac_cif* cif_ = lac_cif_prep_var(thunk, i - n, types);

		ffi_call(&cif_->cif, thunk->sym, lac_variant_address(result), addr);
	}
	*/
	// free pointer args???
}

void lac_execute(lac_stream* fp)
{
	lac_token t;
	lac_variant v;

	for (t = lac_stream_token_next(fp);
	     !lac_token_last(t);
		 t = lac_stream_token_next(fp))
	{
		ensure (!lac_token_error(t));
		const lac_cif* cif = lac_map_get(t);
		ensure (cif);

		lac_call_thunk(fp, &v, cif);
		if (v.type != &ffi_type_void) {
			lac_stack_push(stack, &v);
		}
	}
}

static lac_variant pick(size_t n)
{
	return LAC_STACK_PICK(stack, n, lac_variant);
}
static void push(lac_variant v)
{
	lac_stack_push(stack, &v);
}
static unsigned count(void)
{
	return lac_stack_count(stack);
}
/*
static void print(const lac_variant v)
{
	char fmt[3] = "% ";
	fmt[1] = ffi_type_format(v.type);
}
*/

// add some thunks to the dictionary
void lac_init()
{
	// types
	{
		ffi_type* args[1] = {&ffi_type_sint};
		lac_cif* cif = lac_cif_alloc(&ffi_type_variant, lac_variant_i, 1, args);
		ensure (cif);
		lac_map_put(lac_token_alloc("int", 0), cif);
	}
	{
		lac_cif* cif = lac_cif_load("libc.so.6", &ffi_type_sint, "puts", &ffi_type_pointer, 0);
		ensure (cif);
		lac_map_put(lac_token_alloc("puts", 0), cif);
	}
	{
		lac_cif* cif = lac_cif_load("libc.so.6", &ffi_type_sint, "printf", &ffi_type_pointer, 0);
		ensure (cif);
		// varargs
		cif->cif.nargs = -cif->cif.nargs;
		lac_map_put(lac_token_alloc("printf", 0), cif);
	}
	{
		lac_token key = LAC_TOKEN("type", 0);
		ffi_type* args[1] = {&ffi_type_pointer};
		lac_cif* cif = lac_cif_alloc(&ffi_type_pointer, ffi_type_lookup, 1, args);
		lac_map_put(key, cif);
	}
	{
		lac_token key = LAC_TOKEN("pick",0);
		ffi_type* args[1] = {&ffi_type_uint};
		lac_cif* cif = lac_cif_alloc(&ffi_type_variant, pick, 1, args);
		lac_map_put(key, cif);
	}
	{
		lac_token key = LAC_TOKEN("push",0);
		ffi_type* args[1] = {&ffi_type_variant};
		lac_cif* cif = lac_cif_alloc(&ffi_type_void, push, 1, args);
		lac_map_put(key, cif);
	}
	{
		lac_token key = LAC_TOKEN("depth",0);
		lac_cif* cif = lac_cif_alloc(&ffi_type_uint, count, 0, NULL);
		lac_map_put(key, cif);
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
	lac_execute(&s/*, dict, stack*/);

	LAC_STACK_FREE(stack);

	return 0;
}
