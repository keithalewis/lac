// lac_tree.h - use tsearch instead hash table
#pragma once
#include <stdlib.h>
#include <string.h>

typedef void* lac_tree;

// key and value data
typedef struct {
	char* data;
	size_t size;
} lac_tree_datum;

typedef struct {
	lac_tree_datum key;
	lac_tree_datum val;
	char buf[]; // data for key and val
} lac_tree_node;

static inline lac_tree_node* lac_tree_node_alloc(const lac_tree_datum key, const lac_tree_datum val)
{
	lac_tree_node* node = malloc(sizeof(lac_tree_node) + key.size + val.size);

	if (node) {
		node->key.size = key.size;
		node->key.data = node->buf;
		memcpy(node->key.data, key.data, key.size);

		node->val.size = val.size;
		node->val.data = node->buf + key.size;
		memcpy(node->val.data, val.data, val.size);
	}

	return node;
}

static inline int lac_tree_cmp(const void* a, const void* b)
{
	const lac_tree_datum* ka = &((const lac_tree_node*)a)->key;
	const lac_tree_datum* kb = &((const lac_tree_node*)b)->key;

	int ret = kb->size - ka->size;
	
	if (ret == 0) {
		ret = strncmp(ka->data, kb->data, ka->size);
	}

	return ret;
}

lac_tree lac_tree_create(void);

// add new node and return NULL if exists
lac_tree_node* lac_tree_put(lac_tree* tree, const lac_tree_datum key, const lac_tree_datum val);
// pointer to value corresponding t0 key or NULL if not found
lac_tree_datum* lac_tree_get(const lac_tree* tree, const lac_tree_datum key);
// return pointer to parent node or NULL if not found
lac_tree_node* lac_tree_del(lac_tree* tree, const lac_tree_datum key);

void  lac_tree_destroy(lac_tree);

// call action on each leaf node
void lac_tree_walk(const lac_tree, void(*action)(lac_tree_node*));
