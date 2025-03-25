#ifndef LC_MEM_H
#define LC_MEM_H
#include "lc_types.h"

/**
 * @brief Allocates a block of memory using the default allocator.
 * @note This function ensures the allocated memory is zeroed out to prevent potential memory scraping attacks.
 * 
 * @param size The size of the memory block to allocate.
 * @return lc_void* A pointer to the allocated memory block, or NULL if allocation fails.
 */
lc_void*    lc_mem_alloc (lc_usize size);

/**
 * @brief Frees a block of memory.
 * @param ptr A pointer to a previously allocated memory block.
 */
lc_void     lc_mem_free  (lc_void* ptr);

/**
 * @brief Copies a block of memory from the source to the destination.
 * 
 * @param dst Pointer to the destination memory block.
 * @param src Pointer to the source memory block.
 * @param size Number of bytes to copy.
 */
lc_void     lc_mem_cpy   (lc_void* dst, const lc_void* src, lc_usize size);

#endif