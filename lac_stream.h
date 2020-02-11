// lac_stream.h - FILE or string streams
#pragma once
#include <stdio.h>
#include <string.h>

typedef struct {
	void* p;
	int (*getc)(void*);
} lac_stream;

// return ps->getc(ps->p)
int lac_stream_getc(lac_stream* ps);

// getc functions for lac_stream
int lac_stream_file_getc(void* p);
int lac_stream_string_getc(void* p);

typedef struct {
	char *b; // begin
	char *i; // current pointer
	char *e; // one past last character
} lac_stream_string_view;
// create from lac_token
#define LAC_STREAM_STRING_TOKEN(T) (lac_stream_string_view){T.b, T.b, T.e}

lac_stream lac_stream_file(FILE*);
lac_stream lac_stream_string(lac_stream_string_view *);
