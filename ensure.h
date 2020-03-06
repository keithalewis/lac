// ensure.h - use setjmp/longjmp for error handling
#pragma once
#include <assert.h>
#include <setjmp.h>

extern jmp_buf lac_jmp_buf;
extern const char *lac_strerror;

#define ENSURE_HASH_(x) #x
#define ENSURE_STRZ_(x) ENSURE_HASH_(x)
#define ENSURE_FILE "file: " __FILE__
#ifdef __FUNCTION__
#define ENSURE_FUNC "\nfunction: " __FUNCTION__
#else
#define ENSURE_FUNC ""
#endif
#define ENSURE_LINE "\nline: " ENSURE_STRZ_(__LINE__)
#define ENSURE_SPOT ENSURE_FILE ENSURE_LINE ENSURE_FUNC

#define ensure(e) if (!(e)) { \
		lac_strerror = ENSURE_SPOT "\nensure: \"" #e "\" failed\n"; \
		longjmp(lac_jmp_buf, 1); \
	} else (void)0;
