#include <string.h>
#include "ensure.h"
#include "lac_stack.h"

Xlac_stack* Xlac_stack_alloc(size_t size, size_t size_of)
{
	Xlac_stack* stack = malloc(sizeof(Xlac_stack) + size*size_of);
	ensure (0 != stack);

	stack->sp = stack->data + size*size_of;
	stack->size_of = size_of;
	stack->size = size;

	return stack;
}

Xlac_stack* Xlac_stack_realloc(Xlac_stack* stack, size_t size)
{
	size_t osize = Xlac_stack_size(stack);
	size_t count = Xlac_stack_count(stack);

	if (size > osize) {
		stack = realloc(stack, sizeof(Xlac_stack) + size*stack->size_of);
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

void Xlac_stack_free(Xlac_stack* stack)
{
	free(stack);
}

size_t Xlac_stack_size(Xlac_stack* stack)
{
	return stack->size;
}

size_t Xlac_stack_size_of(Xlac_stack* stack)
{
	return stack->size_of;
}

size_t Xlac_stack_count(Xlac_stack* stack)
{
	return ((stack->data + stack->size*stack->size_of) - stack->sp)/stack->size_of;
}

void Xlac_stack_push(Xlac_stack* stack, void* item)
{
	ensure (Xlac_stack_count(stack) < stack->size);
	stack->sp -= stack->size_of;
	memcpy(stack->sp, item, stack->size_of);
}

void Xlac_stack_pop(Xlac_stack* stack)
{
	ensure (Xlac_stack_count(stack) > 0);
	stack->sp += stack->size_of;
}

void *Xlac_stack_top(Xlac_stack* stack)
{
	return stack->sp;
}
