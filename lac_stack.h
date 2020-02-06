// Xlac_stack.h - generic stack
#include <stdlib.h>

// generic stack growing upward in memory
typedef struct { 
	size_t size;    // number of stack items
	size_t size_of; // sizeof items on stack
	char* sp;       // current stack item
	char data[];    // flexible array
} Xlac_stack;

// allocate size items of size size_of
Xlac_stack* Xlac_stack_alloc(size_t size, size_t size_of);
Xlac_stack* Xlac_stack_realloc(Xlac_stack* stack, size_t size); // stack knows size_of
void Xlac_stack_free(Xlac_stack*);
size_t Xlac_stack_size(Xlac_stack*);
size_t Xlac_stack_size_of(Xlac_stack*);
// current number of items on stack
size_t Xlac_stack_count(Xlac_stack*);
// push size_of bytes on stack
void Xlac_stack_push(Xlac_stack*, void*);
// pop size_of bytes from stack
void Xlac_stack_pop(Xlac_stack*);
// pointer to top of stack
void* Xlac_stack_top(Xlac_stack*);

#define LAC_STACK(S,T) Xlac_stack*
#define LAC_STACK_ALLOC(S,T,N) Xlac_stack * S = Xlac_stack_alloc(N, sizeof(T))
#define LAC_STACK_FREE(S) Xlac_stack_free(S)
#define LAC_STACK_PUSH(S,T) Xlac_stack_push(S, &T)
#define LAC_STACK_POP(S) Xlac_stack_pop(S)
#define LAC_STACK_TOP(S,T) *(T*)Xlac_stack_top(S)


/*
//#define LAC_STACK(T) typedef struct { size_t size; size_t sp; T data[]; } Xlac_stack_##T
#define LAC_STACK_ALLOC(T,N) (Xlac_stack_##T *) Xlac_stack_alloc(N, sizeof(T))
#define LAC_STACK_PUSH(S,T) Xlac_stack_alloc(N, sizeof(T))
*/
