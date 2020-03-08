// lac_token.t.cpp - test parsing
#define _GNU_SOURCE
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ensure.h"
#include "lac_token.h"

/*
#define RAII_VARIABLE(T,V,I,D) \
    void _cleanup_ ## V (T * v) { D(*v); } \
	    T V __attribute__((cleanup(_cleanup_ ## V))) = (I)
*/

// string, type, data, size
#define TOKEN_TEST(X) \
X("", EOF, 0, 0) \
X(" \n\t\ra ", 'a', "a", 1)

// turn string into FILE*
static lac_token lac_token_string(char *s)
{
    lac_token t;

    FILE* is = fmemopen(s, strlen(s), "r");
    t = lac_read_token(is);
    fclose(is);

    return t;
}

static int test_skip_space()
{

    return 0;
}

/*
static int test_next_space()
{
    size_t n;
    char *s;

    s = lac_token_string("\\", &n);
    ensure(1 == n);
    ensure('\\' == s[0]);
    ensure(0 == s[1]);
    free(s);

    s = lac_token_string("\\\\", &n);
    ensure(1 == n);
    ensure('\\' == s[0]);
    ensure(0 == s[1]);
    free(s);

    s = lac_token_string("\\\\\\", &n);
    ensure(2 == n);
    ensure('\\' == s[0]);
    ensure('\\' == s[1]);
    ensure(0 == s[2]);
    free(s);

    s = lac_token_string("\\ ", &n);
    ensure(1 == n);
    ensure(' ' == s[0]);
    ensure(0 == s[1]);
    free(s);

    s = lac_token_string("\\\\ ", &n);
    ensure(2 == n);
    ensure('\\' == s[0]);
    ensure(' ' == s[1]);
    ensure(0 == s[2]);
    free(s);

    return 0;
}
*/
/*
// terminate with null pointer
static int test_lac_token_parse(char *t, ...)
{
    FILE *s = fmemopen(t, strlen(t), "r");

    va_list ap;
    va_start(ap, t);
    char *t_;
    char *u = va_arg(ap, char *);
    size_t n;
    while (u) {
	t_ = lac_token_parse(s, &n);

	if (n == (size_t) EOF) {
	    ensure(0 == strcmp(t_, u));
	}
	else {
	    ensure(strlen(t_) == n)
		ensure(strlen(u) == n)
		ensure(0 == strncmp(t_, u, n));
	    free(t_);
	}

	u = va_arg(ap, char *);
    }

    t_ = lac_token_parse(s, &n);
    // no more tokens
    ensure(n == 0);
    ensure(*t_ == 0);
    free(t_);

    fclose(s);

    return 0;
}
*/
static int test_repl()
{
    lac_token t;
    t = lac_read_token(stdin);
    while (t.type != EOF) {
        printf("%ld: >%s<\n", t.size, t.data);
        fflush(stdout);
        //fsync(1);
        t = lac_read_token(stdin);
    }

    return 0;
}

int test_lac_token();
int test_lac_token()
{
    test_repl();
    //test_skip_space();
    //test_next_space();
    //test_lac_token();
    //test_lac_convert();
/*
    test_lac_token_parse("a b c", "a", "b", "c", 0);
    test_lac_token_parse("a\tb c", "a", "b", "c", 0);
    test_lac_token_parse("a\tb\r c", "a", "b", "c", 0);
    test_lac_token_parse("a\tb\r \nc", "a", "b", "c", 0);
    test_lac_token_parse("\"a b\" c", "a b", "c", 0);
    test_lac_token_parse("{a {b} c}", "a {b} c", 0);
    test_lac_token_parse(" {a {b} c}  d", "a {b} c", "d", 0);
    test_lac_token_parse("%g\n", "%g", 0);
    test_lac_token_parse("\"%g\n\" h", "%g\n", "h", 0);
*/
    return 0;
}

/*
int test_lac_convert()
{
	{
		lac_variant w = lac_variant_parse(&ffi_type_sint, "123");
		ensure (w.type = &ffi_type_sint);
		ensure (w.value._sint == 123);
	}
	{
		lac_variant w = lac_variant_parse(&ffi_type_double, "1.23");
		ensure (w.type = &ffi_type_double);
		ensure (w.value._double == 1.23);
	}
	{
		lac_variant w = lac_variant_parse(&ffi_type_string, "foo bar");
		ensure (w.type = &ffi_type_string);
		ensure (0 == strcmp(w.value._pointer, "foo bar"));
	}

	return 0;
}
*/
