// lac_map.c
#include "lac_map.h"
#include "ensure.h"
#include <search.h> //and use hsearch???
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

struct lac_entry {
    char *key;
    const void *val;
    LIST_ENTRY(lac_entry) st;
    // struct { struct entry* next; struct entry* prev } st;
};

static LIST_HEAD(head, lac_entry) map = LIST_HEAD_INITIALIZER(struct head);

void lac_map_put(const char *key, const void *val)
{
    // static LIST_HEAD(head, entry) map = LIST_HEAD_INITIALIZER(struct head);
    // struct head { struct entry* lh_first} map = { NULL };

    struct lac_entry *elm = malloc(sizeof(struct lac_entry));
    elm->key = strcpy(malloc(strlen(key) + 1), key);
    elm->val = val;
    LIST_INSERT_HEAD(&map, elm, st);
    // LIST_INSERT_HEAD(head, elm, field);
    // head->first = elm
    // elm->field.prev = &head->first
}

static struct lac_entry *lac_map_find(const char *key)
{
    struct lac_entry *elm = 0;

    LIST_FOREACH(elm, &map, st)
    {
        if (0 == strcmp(elm->key, key)) {
            return elm;
        }
    }

    return elm;
}

void lac_map_del(const char *key)
{
    struct lac_entry *elm = lac_map_find(key);

    if (0 != elm) {
        LIST_REMOVE(elm, st);
        free(elm->key);
        free(elm);
    }
}

const void *lac_map_get(const char *key)
{
    struct lac_entry *elm = lac_map_find(key);

    if (0 != elm) {
        return elm->val;
    }

    return 0;
}

void lac_map_foreach(void (*action)(const char *, const void *))
{
    struct lac_entry *elm = 0;

    LIST_FOREACH(elm, &map, st)
    {
        action(elm->key, elm->val);
    }
}
