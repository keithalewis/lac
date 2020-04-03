// lac.c - load and call C functions
//#include <sys/queue.h>
#include "lac.h"

int line = 0;
const char *file = "";

jmp_buf lac_jmp_buf;
const char* lac_strerror = "";

int main(int ac, const char *av[])
{
	// atexit??? on_exit???
	if (setjmp(lac_jmp_buf)) {
		fputs(lac_strerror, stderr);
		lac_strerror = "";
	}

	lac_init();

	FILE *fp;
	//!!! process args to allow multiple input files
	fp = ac > 1 ? fopen(av[1], "r") : stdin;
	ensure(fp);
	file = ac > 1 ? av[1] : "stdin";

	// for file ... lac_execute
	lac_variant result;
	while (!feof(fp)) {
		result = lac_eval(fp); // eval figures out type
		lac_variant_free(&result);
	}

	return result.type == &ffi_type_sint ? result.value._sint : -1;
}
