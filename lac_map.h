// lac_map.h - null terminated char* string to void* val map
#pragma once

// add possibly redundant entry
void lac_map_put(const char* key, const void* val);
// return 0 if entry not found
const void* lac_map_get(const char* key);
// remove entry from map
void lac_map_del(const char* key);
// call free on key
void lac_map_del_k(const char* key);
// call free on value
void lac_map_del_v(const char* key);
// call free on key and value
void lac_map_del_kv(const char* key);
