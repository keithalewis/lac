// lac_map.t.c - test lac_map
#include "ensure.h"
#include "lac_map.h"
#include <stdlib.h>
#include <string.h>

int test_lac_map();
int test_lac_map()
{
    {
        const char key[] = "key";
        char val[] = "val";
        lac_map_put(key, val);
        const char *v = lac_map_get(key);
        ensure(0 == strcmp(v, val));
        v = lac_map_get(key);
        ensure(0 == strcmp(v, val));
        v = lac_map_get("foo");
        lac_map_del(key, 0);
        v = lac_map_get(key);
        ensure(0 == v);
    }
    {
        const char key[] = "key";
        char* val = strdup("val");
        lac_map_put(key, val);
        const char *v = lac_map_get(key);
        ensure(0 == strcmp(v, val));
        v = lac_map_get(key);
        ensure(0 == strcmp(v, val));
        v = lac_map_get("foo");
        lac_map_del(key, free);
        v = lac_map_get(key);
        ensure(0 == v);
    }

    return 0;
}
