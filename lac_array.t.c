// lac_array.t.c - test lac_array
#include <assert.h>
#include "lac_array.h"

int test_lac_array()
{
    {
        lac_array* a = lac_array_make(3);

        assert (a);
        assert ((size_t)-1 == lac_array_item(a)); // empty
        assert (3 == lac_array_size(a));

        assert (0 == lac_array_push(a, (void*)2));
        assert (0 == lac_array_item(a));
        assert ((void*)2 == lac_array_back(a));

        assert (1 == lac_array_push(a, (void*)3));
        assert (1 == lac_array_item(a));
        assert ((void*)3 == lac_array_back(a));

        assert(!lac_array_copy(NULL,a));
        assert(!lac_array_copy(a,NULL));
        assert(!lac_array_copy(NULL,NULL));

        lac_array* a_ = lac_array_make(2);
        assert (a_);
        assert ((size_t)-1 == lac_array_item(a_)); // empty
        assert (2 == lac_array_size(a_));

        lac_array_copy(a_, a);

        lac_array_free(a);
        lac_array_free(a_);
    }

    return 0;
}
