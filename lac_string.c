// lac_string.c
#include <stdio.h>
#include <string.h>
#include "lac_string.h"

struct lac_string_struct {
    FILE* fp;
    size_t size;
    char* data;
};

lac_string lac_string_alloc(const char* s, size_t n)
{
    lac_string_struct* p = malloc(sizeof(struct lac_string_struct));
    p->fp = open_memstream(&p->data, &p->size);
    if (s) {
        if (n == 0) {
            n = strlen(s);
        }
        fwrite(s, n, 1, p->fp);
    }

    return p;
}

lac_string lac_string_append(lac_string p, const char* s, size_t n)
{
    if (n == 0) {
        n = strlen(s);
    }

    fwrite(s, n, 1, p->fp);

    return p;
}

const char* lac_string_data(lac_string p)
{
    fflush(p->fp);

    return p->data;
}

size_t lac_string_size(lac_string)
{
    fflush(p->fp);

    return p->size;
}

void lac_string_free(lac_string p)
{
    fclose(p->fp);
    free(p->data);
}
