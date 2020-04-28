// lac_array.t.c - test lac_array
#include <assert.h>
#include "lac_array.h"

int test_lac_array()
{
    lac_array* a = lac_array_make(3);
    assert (a);
    assert ((size_t)-1 == lac_array_item(a));
    assert (3 == lac_array_size(a));
    assert (0 == lac_array_push(a, (void*)2));
    assert (0 == lac_array_item(a));
    assert ((void*)2 == lac_array_back(a));

    return 0;
}
