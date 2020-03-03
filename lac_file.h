// lac_file.h - file backed key-value store
#pragma once
#include <stdio.h>

// call fopen to get pointer to data in file and size
void* lac_file_get(const char* key, size_t* n);
// write n bytes to file
size_t lac_file_put(const char* key, void* val, size_t n);
