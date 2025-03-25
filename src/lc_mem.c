#include "lc_mem.h"
#include <stdlib.h>
#include <string.h>

void *lc_mem_alloc(lc_usize size)
{
    void *ptr = malloc(size);
    if (!ptr)
        return NULL;

    memset(ptr, 0, size);
    return ptr;
}

lc_void *lc_mem_realloc(lc_void *ptr, lc_usize size)
{
    return realloc(ptr, size);
}

void lc_mem_free(void *ptr)
{
    free(ptr);
}

lc_void lc_mem_copy(lc_void *dst, const lc_void *src, lc_usize size)
{
    memcpy(dst, src, size);
}