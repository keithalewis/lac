#include <string.h>
#include "ensure.h"
#include "lac_stack.h"

lac_stack *lac_stack_alloc(size_t size, size_t size_of)
{
	lac_stack *stack = malloc(sizeof(lac_stack) + size * size_of);
	ensure(0 != stack);

	stack->sp = stack->data + size * size_of;
	stack->size_of = size_of;
	stack->size = size;

	return stack;
}

lac_stack *lac_stack_copy(lac_stack * stack)
{
	lac_stack *copy = lac_stack_alloc(stack->size, stack->size_of);
	ensure(0 != copy);

	size_t n = lac_stack_count(stack) * lac_stack_size_of(stack);
	copy->sp -= n;
	memcpy(copy->sp, stack->sp, n);

	return copy;
}

void lac_stack_free(lac_stack * stack)
{
	free(stack);
}

size_t lac_stack_size(lac_stack * stack)
{
	return stack->size;
}

size_t lac_stack_size_of(lac_stack * stack)
{
	return stack->size_of;
}

size_t lac_stack_count(lac_stack * stack)
{
	return ((stack->data + stack->size * stack->size_of) -
		stack->sp) / stack->size_of;
}

void lac_stack_push(lac_stack * stack, void *item)
{
	ensure(lac_stack_count(stack) < stack->size);
	stack->sp -= stack->size_of;
	memcpy(stack->sp, item, stack->size_of);
}

void lac_stack_pop(lac_stack * stack)
{
	ensure(lac_stack_count(stack) > 0);
	stack->sp += stack->size_of;
}

void *lac_stack_top(lac_stack * stack)
{
	return stack->sp;
}

// get n-th element from stack
void *lac_stack_pick(lac_stack * stack, size_t n)
{
	ensure(0 < n && n <= lac_stack_count(stack));

	return stack->sp + (n - 1) * stack->size_of;
}

// a1 a2 ... an -- an a1 ... an-1
void lac_stack_roll(lac_stack * stack, size_t n)
{
	if (n < 2) {
		return;
	}

	size_t so = stack->size_of;
	char *sn = lac_stack_pick(stack, n);

	char in[so];
	memcpy(in, sn, so);
	memmove(stack->sp + so, stack->sp, (n - 1) * so);
	memcpy(sn, in, so);
}
