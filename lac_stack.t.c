#include <string.h>
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

		{
			lac_stack* s = lac_stack_copy(stack);

			ensure (2 == lac_stack_size(s));
			ensure (1 == lac_stack_size_of(s));
			ensure (1 == lac_stack_count(s));
			ensure (c == *(char*)lac_stack_top(s));

			lac_stack_free(s);
		}
	
		lac_stack_free(stack);
	}
	{
		lac_stack* stack = LAC_STACK_ALLOC(2, double);
		ensure (0 == lac_stack_count(stack));
		ensure (2 == lac_stack_size(stack));
		ensure (sizeof(double) == lac_stack_size_of(stack));

		double d = 1.23;
		lac_stack_push(stack, &d);
		ensure (1 == lac_stack_count(stack));
		ensure (1.23 ==  *(double*)lac_stack_top(stack));
		ensure (1.23 ==  LAC_STACK_TOP(stack, double));
		{
			lac_stack* s = lac_stack_copy(stack);

			ensure (lac_stack_size(stack) == lac_stack_size(s));
			ensure (lac_stack_size_of(stack) == lac_stack_size_of(s));
			ensure (1 == lac_stack_count(s));
			ensure (1.23 == *(double*)lac_stack_top(s));
			ensure (1.23 ==  LAC_STACK_TOP(s, double));

			lac_stack_free(s);
		}
	
		lac_stack_free(stack);
	}
	{
		lac_stack* stack = LAC_STACK_ALLOC(2, char*);
		ensure (0 == lac_stack_count(stack));
		ensure (2 == lac_stack_size(stack));
		ensure (sizeof(char*) == lac_stack_size_of(stack));

		const char* s = "abc";
		lac_stack_push(stack, &s);
		ensure (1 == lac_stack_count(stack));
		char* ps = LAC_STACK_TOP(stack, char*);
		ensure (0 == strcmp(ps, s));
		lac_stack_pop(stack);
		ensure (0 == lac_stack_count(stack));
		lac_stack_push(stack, &s);
		ensure (1 == lac_stack_count(stack));

		char* s_ = malloc(4);
		strcpy(s_, s);
		s_[0] = 'd';
		lac_stack_push(stack, &s_);
		ensure (2 == lac_stack_count(stack));
		char* _s = LAC_STACK_TOP(stack, char*);
		ensure (0 == strcmp(_s, "dbc"));
		s_[1] = 'e';
		ensure (0 == strcmp(_s, "dec"));
		free (_s);
		lac_stack_pop(stack);
		ensure (1 == lac_stack_count(stack));

		LAC_STACK_FREE(stack);
	}
	{
		lac_stack* stack = LAC_STACK_ALLOC(3, long);
		long i1 = 1, i2 = 2;
		lac_stack_push(stack, &i1); // -- i1
		lac_stack_push(stack, &i2); // -- i2 i1
		ensure (2 == lac_stack_count(stack));
		ensure (i2 == LAC_STACK_PICK(stack, 1, long));
		ensure (i1 == LAC_STACK_PICK(stack, 2, long));
/*
		lac_stack_roll(stack, 2); // -- i1 i2
		ensure (2 == lac_stack_count(stack));
		ensure (i1 == LAC_STACK_PICK(stack, 1, long));
		ensure (i2 == LAC_STACK_PICK(stack, 2, long));
*/
		LAC_STACK_FREE(stack);
	}
	
	return 0;
}
