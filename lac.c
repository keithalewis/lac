// lac.c - load and call C functions
//#include <sys/queue.h>
#include "ensure.h"
#include "lac.h"

int line = 0;
const char *file = "";

jmp_buf lac_jmp_buf;
const char* lac_strerror = "";

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
	lac_variant result;
	while (!feof(fp)) {
		result = lac_eval(fp, 0); // eval figures out type
		lac_variant_free(&result);
	}

	return result.type == &ffi_type_sint ? result.value._sint : 0;
}
