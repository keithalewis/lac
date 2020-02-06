#include "ensure.h"
#include "lac_stack.h"

int test_lac_stack()
{
	{
		lac_stack* stack = lac_stack_alloc(2, 1);
		ensure (0 == lac_stack_count(stack));
		ensure (2 == lac_stack_size(stack));
		ensure (1 == lac_stack_size_of(stack));

		char c = 'a';
		lac_stack_push(stack, &c);
		ensure (1 == lac_stack_count(stack));
		char c_ = *(char*)lac_stack_top(stack);
		ensure (c_ == c);
		lac_stack_pop(stack);
		ensure (0 == lac_stack_count(stack));
		lac_stack_push(stack, &c);
		ensure (1 == lac_stack_count(stack));

		stack = lac_stack_realloc(stack, 3);
		{
			lac_stack* s = lac_stack_copy(stack);

			ensure (3 == lac_stack_size(s));
			ensure (1 == lac_stack_size_of(s));
			ensure (1 == lac_stack_count(s));
			ensure (c == *(char*)lac_stack_top(s));

			lac_stack_free(s);
		}
		lac_stack_push(stack, &c);
		ensure (2 == lac_stack_count(stack));

		stack = lac_stack_realloc(stack, 1);
		ensure (1 == lac_stack_count(stack));
		ensure (1 == lac_stack_size(stack));
		ensure (c == *(char*)lac_stack_top(stack));

		lac_stack_free(stack);
	}
	{
		lac_stack* stack = LAC_STACK_ALLOC(2, double);
		ensure (0 == lac_stack_count(stack));

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
		lac_stack_pop(stack);
		ensure (1 == lac_stack_count(stack));
		ensure (1.23 == LAC_STACK_TOP(stack, double));
		lac_stack_pop(stack);
		ensure (0 == lac_stack_count(stack));

		LAC_STACK_FREE(stack);
	}
	
	return 0;
}
