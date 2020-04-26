// lac_array.h - array of pointers
#pragma once
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t item; // next free index
    size_t size; // capacity of array
    void*  data[];
} lac_array;

static inline lac_array* lac_array_make(size_t n)
{
    lac_array* a = malloc(sizeof(lac_array) + n * sizeof(void*));
    if (!a)
        return NULL;

    a->item = 0;
    a->size = n;

    return a;
}

static inline lac_array* lac_array_copy(const lac_array* a)
{
    if (!a)
        return NULL;

    lac_array* a_ = lac_array_make(a->size);
    if (!a_)
        return NULL;

    a_->item = a->item;
    memcpy(a_->data, a->data, a->item*sizeof(void*));

    return a_;
}

static inline void lac_array_free(lac_array* a)
{
    free(a);
}

static inline size_t lac_array_item(const lac_array* a)
{
    if (!a)
        return (size_t)-1;

    return a->item;
}
static inline size_t lac_array_size(const lac_array* a)
{
    if (!a)
        return (size_t)-1;

    return a->size;
}
static inline void* lac_array_data(lac_array* a)
{
    if (!a)
        return NULL;

    return a->data;
}

// add pointer and return index
static inline size_t lac_array_push(lac_array* a, void* p)
{
    if (a->item == a->size)
        return (size_t)-1;

    a->data[a->item] = p;

    return a->item++;
}
