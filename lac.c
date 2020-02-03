// lac.c - load and call C functions
#include <assert.h>
#include <ctype.h>
#include <dlfcn.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gdbm.h>
#include "lac.h"

// use setjmp/longjmp for error handling

lac_dbm dictionary;
lac_dbm library;

lac_stack stack = (lac_stack){STACK_SIZE - 1, STACK_SIZE - 1};

lac_datum make_datum(token_view t)
{
	return (lac_datum){(char*)t.b, t.e - t.b};
}

void evaluate_line(FILE* fp)
{
	//token t = get_token(fp);
	// look up token in dictionary
	// get arguments based on cif
	// call
}

void load_symbol(FILE* fp)
{
	int c;
	char key[64] = "-";
	c = fgetc(fp);
	assert (c == 'l');
	strcat(key, "l");
	c = fgetc(fp);
	while (c != EOF && !isspace(c)) {
		strncat(key, (char*)&c, 1);
	}
	if (c == EOF) {
		exit(EOF);
	}
	//c = skip_space(fp);
	char sym[64] = {(char)c,0};
	// get library name +xx -> lib
	// get symbol name
	// parse remaining args for cif
	// add to dictionary
}

void add_dictionary(FILE* fp)
{
	// get name
	// compile
	// add to dictionary
}

// "-lxxx" -> dlopen("libxxx.so")
void load_library(FILE* fp)
{
	int c;
	char key[64] = "-";
	char lib[1024] = "lib";

	c = fgetc(fp);
	assert (c == 'l');
	strncat(key, (char*)&c, 1);
	
	c = fgetc(fp);
	while (c != EOF && !isspace(c)) {
		strncat(key, (char*)&c, 1);
		strncat(lib, (char*)&c, 1);
		c = fgetc(fp);
	}

	if (c == EOF) {
		exit(EOF);
	}
	
	strcat(lib, ".so");
	void* h = dlopen(lib, RTLD_NOW);
	int ret = lac_dbm_replace(dictionary, (lac_datum){key, strlen(key)-1}, (lac_datum){(char*)h, (int)sizeof(void*)});
}

/*
void evaluate(FILE* fp)
{
	token_view t = get_token(fp);

	if (token_view_empty(t))
		return;

	switch (*t.b) {
		case '-':
			load_library(fp);
			break;
		case '+':
			load_symbol(fp);
			break;
		case ':':
			add_dictionary(fp);
			break;
		default:
			evaluate_line(fp);
	}

	evaluate(fp);
}
*/
#define MAX_LINE 1024
// read line into static buffer
token_view lac_getline(FILE* fp)
{
	static char line[MAX_LINE];

	line[0] = 0;
	char* b = line;
	char* e = line;

	int c = fgetc(fp);
	while (EOF != c) {
		if ('\\' == c) {
			*e++ = c;
			c = fgetc(fp);
			if (EOF == c) {

				break;
			}
		}
		else if ('\n' == c) {

			break;
		}
		else if ('#' == c) {
			do {
				c = fgetc(fp);
			} while (EOF != c && '\n' != c);

			break;
		}

		*e++ = c;
		c = fgetc(fp);
	}
	assert (e - b < LINE_MAX);
	*e = 0;

	return (token_view){b,e};
}

int main(int ac, const char* av[])
{
	FILE* fp;

	// process args
	fp = ac > 1 ? fopen(av[1], "r") : stdin;
	assert (fp);

	dictionary = lac_dbm_open("dictionary");
	library = lac_dbm_open("library");

	// setjmp/longjmp for error handling
	//evaluate(fp);
	while (true) {
		token_view line = lac_getline(fp);
		if (token_view_error(line)) {
			perror(line.b);

			break;
		}
		else if (token_view_empty(line)) {
			perror("empty token_view");

			break;
		}
		else {
			puts(line.b);
		// eval line
		}
	}

	lac_dbm_close(library);
	lac_dbm_close(dictionary);

	return 0;
}
