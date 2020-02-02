// lac_dbm.t.c - test database absraction
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "lac_dbm.h"

#define DATUM(s) (lac_datum){s,strlen(s)}

int test_lac_dbm()
{
	int ret;
	lac_dbm foo = lac_dbm_open("foo");

	ret = lac_dbm_insert(foo, DATUM("key"), DATUM("value"));
	assert (0 == ret);
	lac_datum kv = lac_dbm_fetch(foo, DATUM("key"));
	assert (0 == strcmp(kv.data, "value"));
	free (kv.data);
	kv = lac_dbm_fetch(foo, DATUM("foo"));
	assert (0 == kv.data);

	ret = lac_dbm_save(foo, "bar");
	assert (0 == ret);

	ret = lac_dbm_delete(foo, DATUM("key"));
	assert (0 == ret);
	ret = lac_dbm_delete(foo, DATUM("key"));
	assert (0 != ret);

	lac_dbm bar = lac_dbm_load("bar");
	kv = lac_dbm_fetch(bar, DATUM("key"));
	assert (0 == strcmp(kv.data, "value"));
	free (kv.data);

	lac_dbm_close(foo);
	lac_dbm_close(bar);

	return 0;
}
