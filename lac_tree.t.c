// lac_tree.t.c - test lac_tree
#include <search.h>
#include <stdio.h>
#include "ensure.h"
#include "lac_tree.h"

#define DATUM(s) (lac_tree_datum){.size = strlen(s), .data = s}

static void action(const void *p, const VISIT value, const int level)
{
	const char* s = (*(lac_tree_node**)p)->key.data;
	printf("visit %d, level %d, key %c%c%c\n", value, level, s[0], s[1], s[2]);
}

int test_lac_tree(void)
{
	/*
	{
		lac_tree t = lac_tree_create();

		lac_tree_put(&t, DATUM("foo"), DATUM("val"));
		const lac_tree_datum* val = lac_tree_get(&t, DATUM("foo"));
		ensure (val->size == 3);
		ensure (0 == strncmp(val->data, "val", 3));

		lac_tree_destroy(t);
	}
	*/
	{
		lac_tree t = lac_tree_create();

		ensure (0 != lac_tree_put(&t, DATUM("foo"), DATUM("val")));
		twalk(t, action);
		printf("\n");

		const lac_tree_datum* val = lac_tree_get(&t, DATUM("foo"));
		ensure (val->size == 3);
		ensure (0 == strncmp(val->data, "val", 3));

		ensure (0 != lac_tree_put(&t, DATUM("bar"), DATUM("baz")));
		twalk(t, action);
		printf("\n");
		
		lac_tree_del(&t, DATUM("foo"));
		twalk(t, action);
		printf("\n");

		const lac_tree_datum* val2 = lac_tree_get(&t, DATUM("foo"));
		ensure (val2 == 0);

		lac_tree_destroy(t);
	}
	/*
	{
		//void twalk(const lac_tree, void (*)(const void* p, int value, int level));
		lac_tree t = lac_tree_create();

		lac_tree_put(&t, DATUM("foo"), DATUM("val"));
		//twalk(t, action);
		printf("\n");
		const lac_tree_datum* val = lac_tree_get(&t, DATUM("foo"));
		ensure (val->size == 3);
		ensure (0 == strncmp(val->data, "val", 3));

		lac_tree_put(&t, DATUM("foo"), DATUM("val2"));
		//twalk(t, action);
		printf("\n");
		const lac_tree_datum* val2 = lac_tree_get(&t, DATUM("foo"));
		val2 = val2;
		//ensure (val2->size == 4);
		//ensure (0 == strncmp(val2->data, "val2", 4));

		//twalk(t, action);
		printf("\n");

		lac_tree_destroy(t);
	}
	*/

	return 0;
}
