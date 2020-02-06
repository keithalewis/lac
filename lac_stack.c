#include <string.h>
#include "ensure.h"
#include "lac_stack.h"

lac_stack* lac_stack_alloc(size_t size, size_t size_of)
{
	lac_stack* stack = malloc(sizeof(lac_stack) + size*size_of);
	ensure (0 != stack);

	stack->sp = stack->data + size*size_of;
	stack->size_of = size_of;
	stack->size = size;

	return stack;
}

lac_stack* lac_stack_realloc(lac_stack* stack, size_t size)
{
	size_t osize = lac_stack_size(stack);
	size_t so = lac_stack_size_of(stack);
	size_t count = lac_stack_count(stack);
	long n = size - osize;

	if (n > 0) {
		stack = realloc(stack, sizeof(lac_stack) + size*so);
		ensure (0 != stack);
		memmove(stack->sp + n*so, stack->sp, count*so);
	}
	else if (n < 0) { // n < 0
		if (size <= count) {
			// drop from bottom of stack
			memmove(stack->data, stack->sp, size*so);
			n = (stack->data - stack->sp)/so;
		}
		else {
			memmove(stack->sp + n*so, stack->sp, count*so);
		}
		stack = realloc(stack, sizeof(lac_stack) + size*so);
	}
	stack->sp += n*so;
	stack->size = size;

	return stack;
}

lac_stack* lac_stack_copy(lac_stack* stack)
{
	size_t size = lac_stack_size(stack);
	size_t so = lac_stack_size_of(stack);
	size_t count = lac_stack_count(stack);

	lac_stack* copy = lac_stack_alloc(size, so);
	ensure (0 != copy);

	copy->sp -= count*so;
	memmove(copy->sp, stack->sp, count*so);

	return copy;
}

void lac_stack_free(lac_stack* stack)
{
	free(stack);
}

size_t lac_stack_size(lac_stack* stack)
{
	return stack->size;
}

size_t lac_stack_size_of(lac_stack* stack)
{
	return stack->size_of;
}

size_t lac_stack_count(lac_stack* stack)
{
	return ((stack->data + stack->size*stack->size_of) - stack->sp)/stack->size_of;
}

void lac_stack_push(lac_stack* stack, void* item)
{
	ensure (lac_stack_count(stack) < stack->size);
	stack->sp -= stack->size_of;
	memmove(stack->sp, item, stack->size_of);
}

void lac_stack_pop(lac_stack* stack)
{
	ensure (lac_stack_count(stack) > 0);
	stack->sp += stack->size_of;
}

void *lac_stack_top(lac_stack* stack)
{
	return stack->sp;
}

/*
// s[0],...,s[n] -- s[1],...,s[n],s[0] 
void lac_stack_roll(lac_stack* stack)
{
	char* top[stack->size_of];
	memcpy(top, stack->sp, stack->sizeof);
	for (size_t i = 1; i < lac_stack_count(stack); ++i) {
		memcpy(stack->sp + i - 1, stack->sp
	}
}
*/

// get n-th element from stack
void* lac_stack_pick(lac_stack* stack, size_t n)
{
	ensure (n < lac_stack_count(stack));

	return stack->sp + n*stack->size_of;
}
