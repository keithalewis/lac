// lac_dbm.c
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <gdbm.h>
#include "lac_dbm.h"

static inline datum DATUM(lac_datum d)
{
	return (datum){d.data, d.size};
}

// append ".gdb" to name
static const char* gdb(const char* name)
{
	static char name_gdb[1024];

	strcpy(name_gdb, name);
	strcat(name_gdb, ".gdb");

	return name_gdb;
}

static inline int lac_dbm_flock(lac_dbm dbm, int flag)
{
	int ret;

	errno = 0;
	ret = flock(gdbm_fdesc((GDBM_FILE)dbm), flag);
	if (0 != ret) {
		perror(strerror(errno));
	}

	return ret;
}

lac_dbm lac_dbm_open(const char* name)
{
	return (lac_dbm)gdbm_open(gdb(name), 0, GDBM_NEWDB|GDBM_NOLOCK, 0664, 0);
}

void lac_dbm_close(lac_dbm dbm)
{
	if (dbm) {
		gdbm_close((GDBM_FILE)dbm);
	}
}

int lac_dbm_replace(lac_dbm dbm, lac_datum key, lac_datum val)
{
	int ret;

	//lac_dbm_flock(dbm, LOCK_UN);
	ret = gdbm_store((GDBM_FILE)dbm, DATUM(key), DATUM(val), GDBM_REPLACE);
	//lac_dbm_flock(dbm, LOCK_EX);

	return ret;
}
int lac_dbm_insert (lac_dbm dbm, lac_datum key, lac_datum val)
{
	int ret;

	//lac_dbm_flock(dbm, LOCK_UN);
	ret = gdbm_store((GDBM_FILE)dbm, DATUM(key), DATUM(val), GDBM_INSERT);
	//lac_dbm_flock(dbm, LOCK_EX);

	return ret;
}

lac_datum lac_dbm_fetch(lac_dbm dbm, lac_datum key)
{
	datum val = gdbm_fetch((GDBM_FILE)dbm, DATUM(key));

	return (lac_datum){val.dptr, val.dsize};
}

int lac_dbm_delete(lac_dbm dbm, lac_datum key)
{
	int ret;

	//lac_dbm_flock(dbm, LOCK_UN);
	ret = gdbm_delete((GDBM_FILE)dbm, DATUM(key));
	//lac_dbm_flock(dbm, LOCK_EX);

	return ret;
}

int lac_dbm_dump(lac_dbm dbm, const char* file)
{
	int ret;
	
	ret = gdbm_dump((GDBM_FILE)dbm, gdb(file),
	                  GDBM_DUMP_FMT_ASCII, GDBM_NEWDB, 0600);
	if (0 != ret) {
		perror(gdbm_strerror(gdbm_errno));
	}

	return ret;
}

lac_dbm lac_dbm_load(const char* file)
{
	unsigned long e;
	lac_dbm dbm = 0;

	int ret = gdbm_load((GDBM_FILE*)&dbm, gdb(file),
	                     GDBM_REPLACE, 0, &e);
	if (0 != ret) {
		perror(gdbm_strerror(gdbm_errno));

		return 0;
	}

	return dbm;
}
