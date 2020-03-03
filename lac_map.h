// lac_map.h - string key to void* val map
#pragma once

// add possibly redundant entry
extern void lac_map_put(const char*, const void *val);
// return 0 if entry not found
extern const void *lac_map_get(const char*);
// remove entry from map
extern void lac_map_del(const char*);
// call action on each key-value pair in map
extern void lac_map_foreach(void (*action)(const char*, const void *));
