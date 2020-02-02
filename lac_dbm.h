// lac_dbm.h - key-value map abstraction
#pragma once

// key and value type
typedef struct
{
  char *data;
  int   size;
} lac_datum;

// opaque handle
typedef void* lac_dbm;

// handle to key-value store
lac_dbm lac_dbm_open (const char* name);
// must be closed to free memory
void    lac_dbm_close(lac_dbm dbm);

// return 0 on success
int   lac_dbm_replace(lac_dbm dbm, lac_datum key, lac_datum val);
int   lac_dbm_insert (lac_dbm dbm, lac_datum key, lac_datum val);

// value.data = 0 on failure
// value.data must be freed after fetch
lac_datum lac_dbm_fetch (lac_dbm dbm, lac_datum key);
int       lac_dbm_delete(lac_dbm dbm, lac_datum key);

// save/load database to file
int lac_dbm_save(lac_dbm dbm, const char* file);
lac_dbm lac_dbm_load(const char* file);
