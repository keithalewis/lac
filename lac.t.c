// lac.t.cpp
#include "lac.h"
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

int test_lac_ffi(void);
int test_lac_parse(void);
int test_lac_map(void);
int test_lac(void);
int test_lac_variant(void);
int test_lac_tree(void);
int test_lac_data(void);
int test_lac_token(void);
int test_lac_eval(void);

// call f n times and return millisecnds
/*
static int timer(void(*f)(void), int n)
{
        clock_t b = clock();
        for (int i = 0; i < n; ++i) {
                f();
        }
        clock_t e = clock();

        return ((e - b)*1000)/CLOCKS_PER_SEC;
}
*/

int test_lac()
{
    return 0;
}

jmp_buf lac_jmp_buf;
const char *lac_strerror;

int main()
{
    if (setjmp(lac_jmp_buf)) {
        fputs(lac_strerror, stderr);

        return -1;
    }

    test_lac_parse();
    test_lac_variant();
    test_lac_map();
    test_lac_ffi();
    test_lac_token();
    test_lac_eval();
    // test_lac();
    // test_lac_tree();
    // test_lac_data();

    return 0;
}
