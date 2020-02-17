// lac_map.h - string key to void* val map
#pragma once

// add possibly redundant entry
void lac_map_put(const char*, const void *val);
// return 0 if entry not found
const void *lac_map_get(const char*);
// remove entry from map
void lac_map_del(const char*);
