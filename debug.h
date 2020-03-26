// debug.h
#pragma once
#include <stdio.h>

#define DEBUG_(fmt, ...) \
fprintf(stderr, " file: %s line: %d, func: %s\ndebug: " fmt, \
__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);
