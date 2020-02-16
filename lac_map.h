// lac_map.h - string key to void* val map
#pragma once
#include "lac_parse.h"

// add possibly redundant entry
void lac_map_put(const lac_token, const void *val);
// return 0 if entry not found
const void *lac_map_get(const lac_token);
// remove entry from map
void lac_map_del(const lac_token);
// call free on key
void lac_map_del_k(const lac_token);
// call free on value
void lac_map_del_v(const lac_token);
// call free on key and value
void lac_map_del_kv(const lac_token);
