// lac_date.t.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ensure.h"
#include "lac_data.h"

int test_lac_key()
{
	lac_data* foo = lac_data_alloc(sizeof("foo"), "foo");
	lac_data* foo2 = lac_data_alloc(strlen("foo"), "foo");
	lac_data* foo3 = lac_data_alloc(3, "foo");
	lac_data* bar = lac_data_alloc(3, "bar");

	ensure (0 == lac_data_cmp(foo, foo));
	ensure (0 != lac_data_cmp(foo, foo2));
	ensure (0 == lac_data_cmp(foo2, foo3));
	ensure (foo2->data != foo3->data);
	ensure (0 != lac_data_cmp(foo, bar));

	free(foo);
	free(foo2);
	free(foo3);
	free(bar);

	return 0;
}

int test_lac_data()
{
	test_lac_key();

	return 0;
};
