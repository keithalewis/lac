/* lac.h - load and call C functions */
#pragma once

#include "ensure.h"
#include "lac_ffi.h"
#include "lac_map.h"
#include "lac_parse.h"
#include "lac_stack.h"
#include "lac_stream.h"
#include "lac_variant.h"

typedef struct {
	lac_stream* stream;
	lac_stack* stack;
	lac_map* string;
	lac_map* block;
	lac_map* dict;
} lac_environment;

extern lac_stack* stack;
void lac_init(void);
