#include "ensure.h"
#include "lac_stack.h"

int test_Xlac_stack()
{
	{
		Xlac_stack* stack = Xlac_stack_alloc(2, 1);
		ensure (0 == Xlac_stack_count(stack));

		char c = 'a';
		Xlac_stack_push(stack, &c);
		ensure (1 == Xlac_stack_count(stack));
		char c_ = *(char*)Xlac_stack_top(stack);
		ensure (c_ == c);
		Xlac_stack_pop(stack);
		ensure (0 == Xlac_stack_count(stack));

		Xlac_stack_free(stack);
	}
	{
		LAC_STACK_ALLOC(stack, double, 2);
		ensure (0 == Xlac_stack_count(stack));
		//LAC_STACK_FREE(stack);
		double x = 1.23;
		LAC_STACK_PUSH(stack, x);
		ensure (1 == Xlac_stack_count(stack));
		double y = LAC_STACK_TOP(stack, double);
		ensure (x == y);
		ensure (x == LAC_STACK_TOP(stack, double));
		x *= 2;
		LAC_STACK_PUSH(stack, x);
		ensure (x == LAC_STACK_TOP(stack, double));
		ensure (2 == Xlac_stack_count(stack));
		LAC_STACK_POP(stack);
		ensure (1 == Xlac_stack_count(stack));
		ensure (1.23 == LAC_STACK_TOP(stack, double));
		LAC_STACK_POP(stack);
		ensure (0 == Xlac_stack_count(stack));

		LAC_STACK_FREE(stack);
	}
	
	return 0;
}
