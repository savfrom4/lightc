#include "lc_mem.h"
#include <stdlib.h>
#include <string.h>

lc_void *lc_mem_alloc(lc_usize size)
{
    lc_void *ptr = malloc(size);
    if (!ptr)
        return NULL;

    memset(ptr, 0, size);
    return ptr;
}

lc_void *lc_mem_dupe(const lc_void *src, lc_usize size)
{
    lc_void *new_ptr = lc_mem_alloc(size);
    if (!new_ptr)
        return NULL;

    lc_mem_copy(new_ptr, src, size);
    return new_ptr;
}

lc_void *lc_mem_realloc(lc_void *ptr, lc_usize size)
{
    return realloc(ptr, size);
}

lc_void lc_mem_free(lc_void *ptr)
{
    free(ptr);
}

lc_void lc_mem_copy(lc_void *dst, const lc_void *src, lc_usize size)
{
    memcpy(dst, src, size);
}