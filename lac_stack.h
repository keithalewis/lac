// lac_stack.h - generic stack
#pragma once
#include <stdlib.h>

// generic stack growing upward in memory
typedef struct { 
	size_t size;    // number of stack items
	size_t size_of; // sizeof items on stack
	char* sp;       // current stack item
	char data[];    // flexible array
} lac_stack;

// allocate size items of size size_of
lac_stack* lac_stack_alloc(size_t size, size_t size_of);
void       lac_stack_free(lac_stack*);
lac_stack* lac_stack_copy(lac_stack* stack);

size_t lac_stack_size(lac_stack*);
size_t lac_stack_size_of(lac_stack*);

// current number of items on stack
size_t lac_stack_count(lac_stack*);
// push size_of bytes on stack
void lac_stack_push(lac_stack*, void*);
// pop size_of bytes from stack
void lac_stack_pop(lac_stack*);
// pointer to top of stack
void* lac_stack_top(lac_stack*);

// get n-th element from stack where n = 1 is top of stack
void* lac_stack_pick(lac_stack*,size_t n);
// a1 a2 ... an -- an a1 ... an-1
void lac_stack_roll(lac_stack*,size_t n);

#define LAC_STACK_ALLOC(N,T) lac_stack_alloc(N, sizeof(T))
#define LAC_STACK_FREE(S) lac_stack_free(S)
#define LAC_STACK_PUSH(S,T) lac_stack_push(S, &T)
// cast top of stack to the right type
#define LAC_STACK_TOP(S,T) *(T*)lac_stack_top(S)
#define LAC_STACK_PICK(S,N,T) *(T*)lac_stack_pick(S,N)
