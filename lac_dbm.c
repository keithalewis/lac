// lac_dbm.c
#include <stdlib.h>
#include <string.h>
#include <gdbm.h>
#include "lac_dbm.h"

static inline datum DATUM(lac_datum d)
{
	return (datum){d.data, d.size};
}

static const char* gdb(const char* name)
{
	static char name_gdb[1024];

	strcpy(name_gdb, name);
	strcat(name_gdb, ".gdb");

	return name_gdb;
}

lac_dbm lac_dbm_open(const char* name)
{
	return (lac_dbm)gdbm_open(gdb(name), 0, GDBM_NEWDB, 0664, 0);
}

void lac_dbm_close(lac_dbm dbm)
{
	if (dbm) {
		gdbm_close((GDBM_FILE)dbm);
	}
}

int lac_dbm_replace(lac_dbm dbm, lac_datum key, lac_datum val)
{
	return gdbm_store((GDBM_FILE)dbm, DATUM(key), DATUM(val), GDBM_REPLACE);
}
int lac_dbm_insert (lac_dbm dbm, lac_datum key, lac_datum val)
{
	return gdbm_store((GDBM_FILE)dbm, DATUM(key), DATUM(val), GDBM_INSERT);
}

lac_datum lac_dbm_fetch(lac_dbm dbm, lac_datum key)
{
	datum val = gdbm_fetch((GDBM_FILE)dbm, DATUM(key));

	return (lac_datum){val.dptr, val.dsize};
}

int lac_dbm_delete(lac_dbm dbm, lac_datum key)
{
	return gdbm_delete((GDBM_FILE)dbm, DATUM(key));
}

int lac_dbm_dump(lac_dbm dbm, const char* file)
{
	int ret = gdbm_dump((GDBM_FILE)dbm, gdb(file),
	                  GDBM_DUMP_FMT_ASCII, GDBM_NEWDB, 0600);
	if (0 != ret) {
		puts(gdbm_db_strerror((GDBM_FILE)dbm));
	}

	return ret;
}

lac_dbm lac_dbm_load(const char* file)
{
	unsigned long e;
	lac_dbm dbm = lac_dbm_open(gdb(file));

	if (0 == dbm) {
		return dbm;
	}

	int ret = gdbm_load((GDBM_FILE*)&dbm, gdb(file),
	                     GDBM_REPLACE,
						 GDBM_META_MASK_MODE | GDBM_META_MASK_OWNER, &e);
	if (0 != ret) {
		return 0;
	}

	return dbm;
}
