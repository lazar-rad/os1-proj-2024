#include "../h/memoryManager.hpp"
#include "../h/syscall_c.h"

uint64 MSH::merge()
{
    size_t mergeCounter = 0;

    while ((uint64)this->next && (uint64)this + this->size * MEM_BLOCK_SIZE == (uint64)this->next)
    {
        this->size += this->next->size;
        this->next = this->next->next;
        if (this->next) this->next->prev = this;
        mergeCounter++;
    }

    return mergeCounter;
}

bool MSH::reduce(size_t by, MSH** pHead)
{
    if (this->size < 1 || this->size <= by) return false;

    MSH* newHeader = (MSH*)((uint64)this + by*MEM_BLOCK_SIZE);

    newHeader->size = this->size - by;
    this->size = by;

    newHeader->prev = this->prev;
    newHeader->next = this->next;
    if (*pHead == this) *pHead = newHeader;
    else newHeader->prev->next = newHeader;
    if (newHeader->next) newHeader->next->prev = newHeader;

    this->prev = this->next = nullptr;
    
    return true;
}

void MSH::insertAfter(MSH* other)
{
    if (other)
    {
        other->prev = this;
        other->next = this->next;
        this->next = other;
        if (other->next) other->next->prev = other;
    }
}

void MSH::insertAsHead(MSH** pHead, MSH* other)
{
    if (pHead && other)
    {
        other->prev = nullptr;
        other->next = *pHead;
        *pHead = other;
        if (other->next) other->next->prev = other;
    }
}

void MSH::remove(MSH** pHead)
{
    if (*pHead == this)
    {
        *pHead = this->next;
        if (this->next) this->next->prev = nullptr;
    }
    else
    {
        this->prev->next = this->next;
        if (this->next) this->next->prev = this->prev;
    }
    this->prev = this->next = nullptr;
}

MSH* MSH::findByPointer(MSH* head, MSH* other)
{
    MSH* iterator = nullptr;
    if (head && head <= other)
    {
        iterator = head;
        while (iterator->next && iterator->next <= other)
            iterator = iterator->next;
    }
    return iterator;
}

MSH* MSH::findBySize(MSH* head, size_t nBlocksAct)
{
    MSH* iterator = head;
    while (iterator && iterator->size < nBlocksAct)
        iterator = iterator->next;
    return iterator;
}



MSH* MemoryManager::freeMemoryHead = nullptr;
MSH* MemoryManager::systemMemoryHead = nullptr;
MSH* MemoryManager::userMemoryHead = nullptr;

void MemoryManager::init()
{
    freeMemoryHead = (MSH*)ALIGN((size_t)HEAP_START_ADDR, MEM_BLOCK_SIZE);
    freeMemoryHead->prev = nullptr;
    freeMemoryHead->next = nullptr;
    freeMemoryHead->size = ((size_t)HEAP_END_ADDR - (size_t)freeMemoryHead) / MEM_BLOCK_SIZE;
}

void* MemoryManager::memAlloc_(size_t nBlocks, MSH*& head)
{
    // find free memory
    size_t nBlocksAct = nBlocks+1;
    MSH* memSeg = MSH::findBySize(freeMemoryHead, nBlocksAct);

    if (!memSeg) return nullptr;

    // remove from free memory
    if (memSeg->size == nBlocksAct)
        memSeg->remove(&freeMemoryHead);
    else
        memSeg->reduce(nBlocksAct, &freeMemoryHead);

    // add to process' memory
    MSH* where = MSH::findByPointer(head, memSeg);

    if (where)
        where->insertAfter(memSeg);
    else
        MSH::insertAsHead(&head, memSeg);

    return (void*)((uint64)memSeg + MEM_BLOCK_SIZE);
}

uint64 MemoryManager::memFree_(void* memAddr, MSH*& head)
{
    if (!memAddr) return 0;

    // find memory segment

    MSH* memSeg = (MSH*)((uint64)memAddr - MEM_BLOCK_SIZE);
    if (memSeg != MSH::findByPointer(head, memSeg)) return ERR_MEM_FREE;

    // remove from process

    memSeg->remove(&head);

    // add to free memory

    MSH* where = MSH::findByPointer(freeMemoryHead, memSeg);

    if (where)
        where->insertAfter(memSeg);
    else
        MSH::insertAsHead(&freeMemoryHead, memSeg);

    // merge

    memSeg->merge();
    if (memSeg->prev) memSeg->prev->merge();

    return 0;
}