// lac.t.cpp
#include <math.h>
#include <dlfcn.h>
#include "lac_dbm.h"
#include "lac_ffi.h"
#include "lac_parse.h"

int main()
{
	int test_lac_dbm(void);
	int test_lac_ffi(void);
	int test_lac_parse(void);

	test_lac_dbm();
	test_lac_ffi();
	test_lac_parse();

	return 0;
}
