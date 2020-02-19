// lac.t.cpp
#include "lac.h"

jmp_buf lac_jmp_buf;
const char* lac_strerror;

int test_lac_ffi(void);
int test_lac_parse(void);
int test_lac_map(void);
int test_lac(void);
int test_lac_variant(void);

int main()
{
	if (setjmp(lac_jmp_buf)) {
		fputs(lac_strerror, stderr);
		
		return -1;
	}

	//test_lac_ffi();
	test_lac_parse();
	test_lac_map();
//      test_lac();
	test_lac_variant();

	return 0;
}
