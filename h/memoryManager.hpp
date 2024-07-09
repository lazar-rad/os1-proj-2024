#ifndef __MEMORY__MANAGER__HPP__
#define __MEMORY__MANAGER__HPP__

#include "../lib/hw.h"

#include "../kerneltest/testers.hpp"

#define ALIGN(addr, measure) (((addr+measure-1)/measure)*measure)
#define ROUNDUP(size, block) ((size+block-1)/block)

struct MSH // Memory Segment Header
{
    MSH* prev;
    MSH* next;
    size_t size; // total size of associated space in blocks
    
    uint64 merge();
    // attempts to merge `this` with following segments
    // returns the number of mergers

    bool reduce(size_t by, MSH** head);
    // reduces the segment by `by` blocks from the beginning

    void insertAfter(MSH* other);
    // insert `other` after `this`

    static void insertAsHead(MSH** pHead, MSH* other);

    void remove(MSH** pHead);
    // presumes that `pHead` points to the head of the list containing `this`

    static MSH* findByPointer(MSH* head, MSH* other);
    // presumes a list of ascending pointers
    // returns the last one that precedes or is equal to `other`; if none is found, returns NULL

    static MSH* findBySize(MSH* head, size_t nBlocksAct);
    // returns the first one that is at least `nBlocksAct` in size;
    // if none is of enough size, returns NULL
};

class MemoryManager
{
public:

    static void init();

    inline static void*  memAlloc(size_t nBlocks)
    {
        return memAlloc_(nBlocks, userMemoryHead);
    }

    inline static uint64 memFree(void* memAddr)
    {
        return memFree_(memAddr, userMemoryHead);
    }

    inline static void*  memAlloc_byte(size_t nBytes)
    {
        return memAlloc(ROUNDUP(nBytes, MEM_BLOCK_SIZE));
    }

    inline static void*  kMemAlloc(size_t nBlocks)
    {
        return memAlloc_(nBlocks, systemMemoryHead);
    }

    inline static uint64 kMemFree(void* memAddr)
    {
        return memFree_(memAddr, systemMemoryHead);
    }

    inline static void*  kMemAlloc_byte(size_t nBytes)
    {
        return kMemAlloc(ROUNDUP(nBytes, MEM_BLOCK_SIZE));
    }

private:
    static void*  memAlloc_(size_t nBlocks, MSH*& head);
    static uint64 memFree_(void* memAddr, MSH*& head);

    static MSH* freeMemoryHead;
    static MSH* systemMemoryHead;
    static MSH* userMemoryHead;

#if USE_MEMORY_MANAGER_TESTER == 1
    friend class MemoryManagerTester;
#endif
#if USE_TCB_TESTER == 1
    friend class TCBTester;
#endif
};

#endif // __MEMORY__MANAGER__HPP__