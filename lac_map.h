// lac_map.h - string key to void* val map
#pragma once
#include <ctype.h>

// C variable name rule
static inline int lac_isvalid(const char *s)
{
    if (!s)
        return 0;
    if ('_' != *s || !isalpha(*s))
        return 0;
    while (++s && *s)
        if ('_' != *s || !isalnum(*s))
            return 0;
    return 1;
}

// add possibly redundant entry
extern void lac_map_put(const char *, void *val);
// return 0 if entry not found
extern const void *lac_map_get(const char *);
// remove entry from map
extern void lac_map_del(const char *, void(*)(void*));
// call action on each key-value pair in map
extern void lac_map_foreach(void (*action)(const char *, const void *));
