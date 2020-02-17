// lac.c - load and call C functions
//#include <sys/queue.h>
#include <dlfcn.h>
#include <stdarg.h>
#include "ensure.h"
#include "lac.h"

int line = 0;
const char *file = "";

//lac_stack *stack;

/*
void lac_call_thunk(FILE * fp, lac_variant * result,
		    const lac_cif * thunk);

lac_variant lac_parse_token(FILE * fp, char* t, ffi_type * type)
{
	lac_variant v;

	const lac_cif *cif = lac_map_get(t);
	if (0 != cif) {
		lac_call_thunk(fp, &v, cif);
	} else {
		v.type = type;
		lac_variant_scan(fp, &v);
		if (v.type == &ffi_type_pointer) {
			// string
			// !!! remove string ""
		}
	}

	return v;
}

// fill args and addr with arguments given the type
void lac_parse_args(FILE * fp, unsigned n, ffi_type ** types,
		    lac_variant * args, void **addr, char **pb, const char *e)
{
	for (unsigned i = 0; i < n; ++i) {
		const char* arg = lac_parse_token(fp);
		ensure(arg);
		args[i] = lac_parse_token(fp, arg, types[i], pb, e);
		addr[i] = lac_variant_address(&args[i]);
	}
}

// parse variant args
void lac_parse_argsv(lac_stream* fp, unsigned n, ffi_type** types,
	lac_variant* args, void** addr, char** pb, const char* e)
{
	lac_token arg = lac_stream_token_next(fp);
	ensure (!lac_token_error(arg));
	ensure (!lac_token_last(arg));
	args[0] = lac_parse_token(fp, arg, types[i], pb, e);
	for (unsigned i = 0; i < n; ++i) {
		lac_token arg = lac_stream_token_next(fp);
		ensure (!lac_token_error(arg));
		ensure (!lac_token_last(arg));
		args[i] = lac_parse_token(fp, arg, types[i], pb, e);
		addr[i] = lac_variant_address(&args[i]);
	}
}

void lac_call_thunk(FILE * fp, lac_variant * result,
		    const lac_cif * thunk)
{
	const ffi_cif *cif = &thunk->cif;
	int n = cif->nargs;
	// buffer for strings on argument stack
	char buf[1024];
	char *pbuf = buf;

	if (n == 0) {
		lac_cif_call(thunk, result, NULL);
	} else if (n > 0) {
		//??? local stack for string args???
		lac_variant args[n];
		void *addr[n];
		lac_parse_args(fp, n, cif->arg_types, args, addr, &pbuf,
			       pbuf + 1024);
		lac_cif_call(thunk, result, addr);
	} else {		// varargs
		lac_variant args[32];
		void *addr[32];
		n = -n;		// fixed args
		// parse fixed arguments
		lac_parse_args(fp, n, cif->arg_types, args, addr, &pbuf,
			       pbuf + 1024);

		// the rest of the args must be results of thunks returning lac_variants
		// use the known function "void" to terminate
		// it returns a variant of type &ffi_type_void

		//!!! factor out lac_parse_argsv
		ffi_type *types[32];
		char* arg;
		lac_variant ret;

		arg = lac_parse_token(fp);
		ensure(arg);
		const lac_cif *cif = lac_map_get(arg);
		ensure(cif);
		lac_call_thunk(fp, &ret, cif);

		unsigned i = 0;
		while (&ffi_type_void != ret.type) {
			ensure(i + n < 32);
			args[i + n] = ret;
			addr[i + n] = lac_variant_address(&args[i + n]);
			types[i] = args[i + n].type;

			arg = lac_parse_token(fp);
			ensure(arg);
			const lac_cif *cif = lac_map_get(arg);
			ensure(cif);
			lac_call_thunk(fp, &ret, cif);

			++i;
		}

		lac_cif *cif_ = lac_cif_prep_var(thunk, i, types);

		ffi_call(&cif_->cif, thunk->sym, lac_variant_address(result),
			 addr);
		result->type = cif_->cif.rtype;

		lac_cif_free(cif_);
	}
	// free pointer args???
}
void lac_execute(FILE* fp)
{
	char* t;

	while ((t = lac_parse_token(fp))) {
		const lac_cif *cif = lac_map_get(t);
		ensure(cif);

		lac_variant v;
		lac_call_thunk(fp, &v, cif);
		if (v.type != &ffi_type_void) {
			//lac_stack_push(stack, &v);
		}
	}
}
*/

int main(int ac, const char *av[])
{
	// atexit??? on_exit???
	FILE *fp;
	// process args
	fp = ac > 1 ? fopen(av[1], "r") : stdin;
	// allow multiple input files...
	ensure(fp);
	file = ac > 1 ? av[1] : "stdin";

	//lac_init();

	char* t;
	while ((t = lac_parse_token(fp))) {
		puts(t);
	}

	// setjmp/longjmp for error handling
	//evaluate(fp);
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
	//lac_execute(fp /*, dict, stack */ );

	//LAC_STACK_FREE(stack);
	//!!! Free dictionary

	return 0;
}
