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
	size_t count = lac_stack_count(stack);

	if (size > osize) {
		stack = realloc(stack, sizeof(lac_stack) + size*stack->size_of);
		ensure (0 != stack);
		memcpy(stack->sp + (size - osize)*stack->size_of, stack->sp, count*stack->size_of);
		stack->sp += (size - osize)*stack->size_of;
	}
	else if (size < osize) {
		if (count > osize - size) {
			// this will discard items on the bottom of the stack
			count = osize - size;
		}
		memcpy(stack->sp - count*stack->size_of, stack->sp, count*stack->size_of);
	}
	stack->size = size;

	return stack;
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
	memcpy(stack->sp, item, stack->size_of);
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
