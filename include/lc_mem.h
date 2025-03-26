#ifndef LC_MEM_H
#define LC_MEM_H
#include "lc_types.h"

lc_void *lc_mem_alloc(lc_usize size);
lc_void *lc_mem_dupe(const lc_void *src, lc_usize size);
lc_void *lc_mem_realloc(lc_void *ptr, lc_usize size);
lc_void lc_mem_free(lc_void *ptr);
lc_void lc_mem_copy(lc_void *dst, const lc_void *src, lc_usize size);

#endif