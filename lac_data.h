// lac_data.h
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// counted strings are used as dictionary keys
typedef struct {
	size_t size;
	uint8_t data[];
} lac_data;

// allocate lac_data and copy data
// use free() to free
static inline lac_data* lac_data_alloc(size_t size, const uint8_t* data)
{
	lac_data* p = malloc(sizeof(lac_data) + size);

	if (p) {
		p->size = size;
		memcpy(p->data, data, size);
	}

	return p;
}

static inline int lac_data_cmp(const lac_data* a, const lac_data* b)
{
	int ret = b->size - a->size;

	if (0 == ret) {
		ret = memcmp(a->data, b->data, a->size);
	}

	return ret;
}

