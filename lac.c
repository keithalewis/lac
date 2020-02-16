// lac.c - load and call C functions
//#include <sys/queue.h>
#include <dlfcn.h>
#include <stdarg.h>
#include "ensure.h"
#include "lac.h"

#define MAX_BUF 1024

int line = 0;
const char *file = "";

lac_stack *stack;

void lac_call_thunk(lac_stream * fp, lac_variant * result,
		    const lac_cif * thunk);

lac_variant lac_parse_token(lac_stream * fp, lac_token t, ffi_type * type,
			    char **pb, const char *e)
{
	lac_variant v;

	const lac_cif *cif = lac_map_get(t);
	if (0 != cif) {
		lac_call_thunk(fp, &v, cif);
	} else {
		v = lac_variant_parse(type, t.b, t.e);
		if (v.type == &ffi_type_pointer) {
			// string
			// !!! remove string ""
			unsigned n = lac_token_size(t);
			ensure(*pb + n + 1 < e);
			strncpy(*pb, v.value.p, n);
			(*pb)[n] = 0;
			v.value.p = *pb;
			*pb += n + 1;
		}
	}

	return v;
}

// fill args and addr with arguments given the type
void lac_parse_args(lac_stream * fp, unsigned n, ffi_type ** types,
		    lac_variant * args, void **addr, char **pb, const char *e)
{
	for (unsigned i = 0; i < n; ++i) {
		lac_token arg = lac_stream_token_next(fp);
		ensure(!lac_token_error(arg));
		ensure(!lac_token_last(arg));
		args[i] = lac_parse_token(fp, arg, types[i], pb, e);
		addr[i] = lac_variant_address(&args[i]);
	}
}

// parse variant args
/*
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
*/

void lac_call_thunk(lac_stream * fp, lac_variant * result,
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
		lac_token arg;
		lac_variant ret;

		arg = lac_stream_token_next(fp);
		ensure(!lac_token_error(arg));
		ensure(!lac_token_last(arg));
		const lac_cif *cif = lac_map_get(arg);
		ensure(cif);
		lac_call_thunk(fp, &ret, cif);

		unsigned i = 0;
		while (&ffi_type_void != ret.type) {
			ensure(i + n < 32);
			args[i + n] = ret;
			addr[i + n] = lac_variant_address(&args[i + n]);
			types[i] = args[i + n].type;

			arg = lac_stream_token_next(fp);
			ensure(!lac_token_error(arg));
			ensure(!lac_token_last(arg));
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

void lac_execute(lac_stream * fp)
{
	lac_token t;

	for (t = lac_stream_token_next(fp);
	     !lac_token_last(t); t = lac_stream_token_next(fp)) {
		ensure(!lac_token_error(t));
		const lac_cif *cif = lac_map_get(t);
		ensure(cif);

		lac_variant v;
		lac_call_thunk(fp, &v, cif);
		if (v.type != &ffi_type_void) {
			lac_stack_push(stack, &v);
		}
	}
}

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
	lac_execute(&s /*, dict, stack */ );

	LAC_STACK_FREE(stack);
	//!!! Free dictionary

	return 0;
}
