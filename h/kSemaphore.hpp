#ifndef __K__SEMAPHORE__HPP__
#define __K__SEMAPHORE__HPP__

#include "../lib/hw.h"
#include "../h/syscall_c.h"

#include "../h/memoryManager.hpp"
#include "../h/tcb.hpp"

class kSemaphore
{
public:
    static kSemaphore* kSemaphoreCreate(uint64 val);
    uint64 close();
    uint64 wait();
    uint64 signal(bool onClose = false);
    uint64 timedWait(time_t timeout);
    uint64 tryWait();

    static void kSemaphoreDeleteAll();

    inline TCB* getCreator() { return creator; }

    void* operator new   (size_t n) { return MemoryManager::kMemAlloc_byte(n); }
    void* operator new[] (size_t n) { return MemoryManager::kMemAlloc_byte(n); }
    void  operator delete   (void* p) noexcept { MemoryManager::kMemFree(p); }
    void  operator delete[] (void* p) noexcept { MemoryManager::kMemFree(p); }

private:
    kSemaphore(uint64 val) : semID(IDCounter++), creator(TCB::running), nextSem(nullptr),
                             value(val), blockedHead(nullptr), blockedTail(nullptr),
                             active(true) { }

    ~kSemaphore() { }

    void put(TCB* tcb);
    TCB* get(TCB* tcb = nullptr);

    inline static uint64 resolveManner()
    {
        switch (TCB::running->unblockManner)
        {
            case TCB::UnblockManner::REGULAR:
                return 0;
            case TCB::UnblockManner::ON_CLOSE:
                return ERR_SEM_WAITCLOSE;
            case TCB::UnblockManner::WAIT_TIMEOUT:
                return TIMEOUT;
            default:
                return 0;
        }
    }

    uint64 semID;
    TCB* creator;
    kSemaphore* nextSem;
    uint64 value;
    TCB* blockedHead;
    TCB* blockedTail;
    bool active;

    static kSemaphore* kSemsHead;
    static kSemaphore* kSemsTail;

    static uint64 IDCounter;

    friend class Sleep;
    friend void TCB::finish(uint64);
};




#endif // __K__SEMAPHORE__HPP__