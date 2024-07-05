#ifndef __STACK__HPP__
#define __STACK__HPP__

#include "../lib/hw.h"

#include "../h/memoryManager.hpp"

struct StackField
{
    uint64 field;

    inline StackField& operator= (uint64 value) { field = value; return *this; }
    inline operator uint64() { return field; }

    void* operator new   (size_t n) { return MemoryManager::kMemAlloc_byte(n); }
    void* operator new[] (size_t n) { return MemoryManager::kMemAlloc_byte(n); }
    void  operator delete   (void* p) noexcept { MemoryManager::kMemFree(p); }
    void  operator delete[] (void* p) noexcept { MemoryManager::kMemFree(p); }
};









#endif // __STACK__HPP__