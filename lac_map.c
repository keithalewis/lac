// lac_map.c
#include <stdlib.h>
#include <string.h>
#include <search.h> //and use hsearch???
#include <sys/queue.h>
#include "ensure.h"
#include "lac_parse.h"

struct lac_entry {
	lac_token key;
	void* val;
	LIST_ENTRY(lac_entry) st;
	// struct { struct entry* next; struct entry* prev } st;
};

static LIST_HEAD(head, lac_entry) map = LIST_HEAD_INITIALIZER(struct head);

void lac_map_put(lac_token key, void* val)
{
	int ret;
	ret = hcreate(100);
	//struct hsearch_data data;
	ensure (ret);
	//static LIST_HEAD(head, entry) map = LIST_HEAD_INITIALIZER(struct head);
	// struct head { struct entry* lh_first} map = { NULL };

	struct lac_entry* elm = malloc(sizeof(struct lac_entry));
	elm->key = key;
	elm->val = val;
	LIST_INSERT_HEAD(&map, elm, st);
	//LIST_INSERT_HEAD(head, elm, field);
	// head->first = elm
	// elm->field.prev = &head->first
}

struct lac_entry* lac_map_find(lac_token key)
{
	struct lac_entry* elm = 0;

	LIST_FOREACH(elm, &map, st) {
		if (lac_token_equal(elm->key, key)) {
			return elm;
		}
	}

	return elm;
}

void lac_map_del(lac_token key)
{
	struct lac_entry* elm = lac_map_find(key);

	if (0 != elm) {
		LIST_REMOVE(elm, st);
		free (elm);
	}
}
void lac_map_del_k(lac_token key)
{
	struct lac_entry* elm = lac_map_find(key);

	if (0 != elm) {
		free ((void*)elm->key.b);
		LIST_REMOVE(elm, st);
		free (elm);
	}
}
void lac_map_del_v(lac_token key)
{
	struct lac_entry* elm = lac_map_find(key);

	if (0 != elm) {
		free (elm->val);
		LIST_REMOVE(elm, st);
		free (elm);
	}
}
void lac_map_del_kv(lac_token key)
{
	struct lac_entry* elm = lac_map_find(key);

	if (0 != elm) {
		free ((void*)elm->key.b);
		free (elm->val);
		LIST_REMOVE(elm, st);
		free (elm);
	}
}

void* lac_map_get(lac_token key)
{
	struct lac_entry* elm = lac_map_find(key);

	if (0 != elm) {
		return elm->val;
	}

	return 0;
}
