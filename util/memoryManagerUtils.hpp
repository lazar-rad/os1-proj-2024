#ifndef __MEMORY__MANAGER__UTILS__HPP__
#define __MEMORY__MANAGER__UTILS__HPP__

#include "../h/memoryManager.hpp"
#include "../lib/hw.h"
#include "../util/arithmetics.hpp"

#define PTR_ACT_TO_GOTTEN(p) (&((uint8*)(p))[MEM_BLOCK_SIZE])
#define PTR_GOTTEN_TO_ACT(p) (&((uint8*)(p))[-MEM_BLOCK_SIZE])

inline uint64 countSegments(const MSH* head)
{
    uint64 b = 0;
    while (head) b++, head = head->next;
    return b;
}

inline uint64 getBlocksCount(const void* p)
{
    return ((MSH*)PTR_GOTTEN_TO_ACT(p))->size;
}

inline size_t totalSize(const MSH* head)
{
    size_t sz = 0;
    while (head) sz += head->size * MEM_BLOCK_SIZE, head = head->next;
    return sz;
}

#endif // __MEMORY__MANAGER__UTILS__HPP__