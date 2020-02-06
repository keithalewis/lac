// lac_stack.h - generic stack
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
lac_stack* lac_stack_realloc(lac_stack* stack, size_t size); // stack knows size_of
void lac_stack_free(lac_stack*);
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

#define LAC_STACK(S,T) lac_stack*
#define LAC_STACK_ALLOC(S,T,N) lac_stack * S = lac_stack_alloc(N, sizeof(T))
#define LAC_STACK_FREE(S) lac_stack_free(S)
#define LAC_STACK_PUSH(S,T) lac_stack_push(S, &T)
#define LAC_STACK_POP(S) lac_stack_pop(S)
#define LAC_STACK_TOP(S,T) *(T*)lac_stack_top(S)


/*
//#define LAC_STACK(T) typedef struct { size_t size; size_t sp; T data[]; } lac_stack_##T
#define LAC_STACK_ALLOC(T,N) (lac_stack_##T *) lac_stack_alloc(N, sizeof(T))
#define LAC_STACK_PUSH(S,T) lac_stack_alloc(N, sizeof(T))
*/
