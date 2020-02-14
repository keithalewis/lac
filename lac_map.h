// lac_map.h - string key to void* val map
#pragma once
#include "lac_parse.h"

/*
typedef struct {
	const char* b;
	const char* e;
};
*/
typedef lac_token lac_map_key;
typedef void* lac_map_val;

typedef void* lac_map;

// add possibly redundant entry
void lac_map_put(const lac_map_key, const void* val);
// return 0 if entry not found
const void* lac_map_get(const lac_map_key);
// remove entry from map
void lac_map_del(const lac_map_key);
// call free on key
void lac_map_del_k(const lac_map_key);
// call free on value
void lac_map_del_v(const lac_map_key);
// call free on key and value
void lac_map_del_kv(const lac_map_key);
