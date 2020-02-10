// lac_map.c
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

struct entry {
	char* key;
	void* val;
	LIST_ENTRY(entry) st;
	// struct { struct entry* next; struct entry* prev } st;
};

static LIST_HEAD(head, entry) map = LIST_HEAD_INITIALIZER(struct head);

void lac_map_put(char* key, void* val)
{
	//static LIST_HEAD(head, entry) map = LIST_HEAD_INITIALIZER(struct head);
	// struct head { struct entry* lh_first} map = { NULL };

	struct entry* elm = malloc(sizeof(struct entry));
	elm->key = key;
	elm->val = val;
	LIST_INSERT_HEAD(&map, elm, st);
	//LIST_INSERT_HEAD(head, elm, field);
	// head->first = elm
	// elm->field.prev = &head->first
}

struct entry* lac_map_find(char* key)
{
	struct entry* elm = 0;

	LIST_FOREACH(elm, &map, st) {
		if (0 == strcmp(elm->key, key)) {
			return elm;
		}
	}

	return elm;
}

void lac_map_del(char* key)
{
	struct entry* elm = lac_map_find(key);

	if (0 != elm) {
		LIST_REMOVE(elm, st);
		free (elm);
	}
}
void lac_map_del_k(char* key)
{
	struct entry* elm = lac_map_find(key);

	if (0 != elm) {
		free (elm->key);
		LIST_REMOVE(elm, st);
		free (elm);
	}
}
void lac_map_del_v(char* key)
{
	struct entry* elm = lac_map_find(key);

	if (0 != elm) {
		free (elm->val);
		LIST_REMOVE(elm, st);
		free (elm);
	}
}
void lac_map_del_kv(char* key)
{
	struct entry* elm = lac_map_find(key);

	if (0 != elm) {
		free (elm->key);
		free (elm->val);
		LIST_REMOVE(elm, st);
		free (elm);
	}
}

void* lac_map_get(char* key)
{
	struct entry* elm = lac_map_find(key);

	if (0 != elm) {
		return elm->val;
	}

	return 0;
}
