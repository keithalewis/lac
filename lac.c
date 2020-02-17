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

// fill args and addr with arguments given the type
void lac_parse_args(FILE* fp, unsigned n, ffi_type ** types,
	lac_variant* args, void** addr)
{
	for (unsigned i = 0; i < n; ++i) {
		args[i].type = types[i];
		lac_variant_scan(fp, &args[i]); //??? error check
		addr[i] = lac_variant_address(&args[i]);
	}
}
/*
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
*/

void lac_call_thunk(FILE * fp, lac_variant * result,
		    const lac_cif * thunk)
{
	ensure (thunk);
	const ffi_cif *cif = &thunk->cif;
	int n = cif->nargs;

	if (n == 0) {
		lac_cif_call(thunk, result, NULL);
	}
	else if (n > 0) {
		//??? local stack for string args???
		lac_variant args[n];
		void *addr[n];
		lac_parse_args(fp, n, cif->arg_types, args, addr);
		lac_cif_call(thunk, result, addr);
	}
	/*
	else {		// varargs
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
	*/
	// free pointer args???
}

lac_variant lac_execute(FILE* fp)
{
	lac_variant v = { .type = &ffi_type_void };

	for (char* t = lac_parse_token(fp); t; t = lac_parse_token(fp)) {
		lac_call_thunk(fp, &v, lac_map_get(t));
	}

	return v;
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

	if (setjmp(lac_jmp_buf)) {
		fputs(lac_strerror, stderr);
		lac_strerror = "";
	}

	// for file ... lac_execute
	lac_execute(fp);

	return 0;
}
