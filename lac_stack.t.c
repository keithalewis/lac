#include "ensure.h"
#include "lac_stack.h"

int test_lac_stack()
{
	{
		lac_stack* stack = lac_stack_alloc(2, 1);
		ensure (0 == lac_stack_count(stack));

		char c = 'a';
		lac_stack_push(stack, &c);
		ensure (1 == lac_stack_count(stack));
		char c_ = *(char*)lac_stack_top(stack);
		ensure (c_ == c);
		lac_stack_pop(stack);
		ensure (0 == lac_stack_count(stack));

		lac_stack_free(stack);
	}
	{
		LAC_STACK_ALLOC(stack, double, 2);
		ensure (0 == lac_stack_count(stack));
		//LAC_STACK_FREE(stack);
		double x = 1.23;
		LAC_STACK_PUSH(stack, x);
		ensure (1 == lac_stack_count(stack));
		double y = LAC_STACK_TOP(stack, double);
		ensure (x == y);
		ensure (x == LAC_STACK_TOP(stack, double));
		x *= 2;
		LAC_STACK_PUSH(stack, x);
		ensure (x == LAC_STACK_TOP(stack, double));
		ensure (2 == lac_stack_count(stack));
		LAC_STACK_POP(stack);
		ensure (1 == lac_stack_count(stack));
		ensure (1.23 == LAC_STACK_TOP(stack, double));
		LAC_STACK_POP(stack);
		ensure (0 == lac_stack_count(stack));

		LAC_STACK_FREE(stack);
	}
	
	return 0;
}
