// lac_token.t.cpp - test parsing
#define _GNU_SOURCE
#include "ensure.h"
#include "lac_token.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
#define RAII_VARIABLE(T,V,I,D) \
    void _cleanup_ ## V (T * v) { D(*v); } \
            T V __attribute__((cleanup(_cleanup_ ## V))) = (I)
*/

// string, type, data, size
#define TOKEN_TEST(X)                                                          \
    X(" \n\t\ra ", 'a', "a", 1)                                                \
    X("\"a b\"", '"', "a b", 3)                                                \
    X(" \"a b\"\r", '"', "a b", 3)                                             \
    X(" {a b}\r", '{', "a b", 3)                                               \
    X("{a {b c} d}", '{', "a {b c} d", 9)                                      \
    X(" {a {b c} d}\r", '{', "a {b c} d", 9)

// turn string into FILE*
static lac_token lac_token_string(char *s)
{
    lac_token t;

    FILE *is = fmemopen(s, strlen(s), "r");
    t = lac_token_read(is);
    fclose(is);

    return t;
}

static int test_skip_space()
{
    lac_token t;
#define X(I, T, D, S)                                                          \
    t = lac_token_string(I);                                                   \
    ensure(t.type == T);                                                       \
    if (D != NULL) {                                                           \
        ensure(0 == strcmp(t.data, D));                                        \
    }                                                                          \
    ensure(t.size == S);
    TOKEN_TEST(X)
#undef X
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
    t = lac_token_read(stdin);
    while (t.type != EOF) {
        printf("%ld: >%s<\n", t.size, t.data);
        fflush(stdout);
        // fsync(1);
        t = lac_token_read(stdin);
    }

    return 0;
}

int test_lac_token();
int test_lac_token()
{
    test_skip_space();
    // test_repl();

    return 0;
}
