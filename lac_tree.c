#define __USE_GNU
#include <search.h>
#include <stddef.h>
#include <string.h>
#include "lac_tree.h"


lac_tree lac_tree_create(void)
{
	return NULL;
}

lac_tree_node* lac_tree_put(lac_tree* tree, const lac_tree_datum key, const lac_tree_datum val)
{
	lac_tree_node* node = lac_tree_node_alloc(key, val);
	if (node) {
		// duplicate keys ok???
		/*lac_tree_node** pfind =*/ tsearch(node, tree, lac_tree_cmp);
	}

	return node;
}
// pointer to value corresponding t0 key or NULL if not found
lac_tree_datum* lac_tree_get(const lac_tree* tree, const lac_tree_datum key)
{
	const lac_tree_node node = {.key = key};
	
	lac_tree_node** find = tfind(&node, tree, lac_tree_cmp);

	return find ? &(*find)->val : 0;
}

// return pointer to parent node or NULL if not found
lac_tree_node* lac_tree_del(lac_tree* tree, const lac_tree_datum key)
{
	const lac_tree_node node = {.key = key};
	
	return tdelete(&node, tree, lac_tree_cmp);
}

// __USE_GNU
void lac_tree_destroy(lac_tree tree)
{
	void tdestroy(void*, void(*)(void*));

	tdestroy(tree, free);
}

// call action on each leaf node
void lac_tree_walk(const lac_tree tree, void(*action)(lac_tree_node*))
{
}
