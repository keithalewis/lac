// lac_stream.h - FILE or string streams
#pragma once
#include <stdio.h>
#include <string.h>

typedef struct {
	char *b; // begin
	char *i; // current pointer
	char *e; // one past last character
} lac_stream;

#define sgetc(S) _Generic((S), FILE*: fgetc, lac_stream*: lac_getc)(S)

#define LAC_STREAM(S, N) (lac_stream){S, S, S + (N == 0 ? strlen(S) : N)}

int lac_getc(lac_stream* ps);
