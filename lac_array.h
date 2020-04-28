// lac_array.h - array of pointers
#pragma once
#include <stdlib.h>
#include <string.h>

#ifdef _DEBUG
extern int test_lac_array();
#endif

typedef struct {
    size_t item; // next free index
    size_t size; // capacity of array
    void*  data[];
} lac_array;

// index of last array entry
static inline size_t lac_array_item(const lac_array* a)
{
    return a ? a->item : (size_t)-1;
}

// array capacity
static inline size_t lac_array_size(const lac_array* a)
{
    return a ? a->size : (size_t)-1;
}

// pointer to initial item
static inline void* lac_array_data(lac_array* a)
{
    return a ? a->data : NULL;
}
// last item in array
static inline void* lac_array_back(lac_array* a)
{
    if (a && a->item != (size_t)-1) {
        return a->data[a->item];
    }

    return NULL;
}

static inline lac_array* lac_array_make(size_t n)
{
    lac_array* a = malloc(sizeof(lac_array) + n * sizeof(void*));
    if (!a)
        return NULL;

    a->item = (size_t)-1;
    a->size = n;

    return a;
}

// copy a to a_ and return a_, or NULL on error
static inline lac_array* lac_array_copy(lac_array* a_, const lac_array* a)
{
    if (!a || !a_)
        return NULL;

    if (a_->size < a->item)
        return NULL;

    if (a->item != (size_t)-1) {
        memcpy(a_->data, a->data, a->item*sizeof(void*));
    }
    a_->item = a->item;

    return a_;
}

static inline void lac_array_free(lac_array* a)
{
    free(a);
}


// push pointer to back of array and return its index
static inline size_t lac_array_push(lac_array* a, void* p)
{
	++a->item;
    if (a->item == a->size)
        return (size_t)-1;

    a->data[a->item] = p;

    return a->item;
}
