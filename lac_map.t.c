// lac_map.t.c - test lac_map
#include <stdlib.h>
#include <string.h>
#include "ensure.h"
#include "lac_map.h"

int test_lac_map()
{
	lac_token key = LAC_TOKEN("key", 0);
	char val[] = "val";
	lac_map_put(key, val);
	const char* v = lac_map_get(key);
	ensure (0 == strcmp(v, val));
	v = lac_map_get(key);
	ensure (0 == strcmp(v, val));
	v = lac_map_get(LAC_TOKEN("foo", 0));
	lac_map_del(key);
	v = lac_map_get(key);
	ensure (0 == v);

	return 0;
}
