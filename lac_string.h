// lac_string.h - dyhamically grow strings
#pragma once

typedef void* lac_string;

lac_string lac_string_alloc(const char*, size_t);
lac_string lac_string_append(lac_string, const char*, size_t);
const char* lac_string_data(lac_string);
size_t lac_string_size(lac_string);
void lac_string_free(lac_string);
