#ifndef __IO__HPP__
#define __IO__HPP__

#include "../lib/hw.h"
#include "../h/syscall_c.h"

#include "../h/memoryManager.hpp"
#include "../h/kernel.hpp"
#include "../h/tcb.hpp"
#include "../h/kSemaphore.hpp"

class CharBuffer
{
public:
    CharBuffer() : pos_w(0), pos_r(0)
    {
        semPut = kSemaphore::kSemaphoreCreate(length);
        semGet = kSemaphore::kSemaphoreCreate(0);
        mutex = kSemaphore::kSemaphoreCreate(1);
    }

    void kPut(char c)
    {
        semPut->wait();
        mutex->wait();
        put(c);
        mutex->signal();
        semGet->signal();
    }

    char kGet()
    {
        semGet->wait();
        mutex->wait();
        char c = get();
        mutex->signal();
        semPut->signal();
        return c;
    }
/*
    void uPut(char c)
    {
        sem_wait((sem_t)semPut);
        sem_wait((sem_t)mutex);
        put(c);
        sem_signal((sem_t)mutex);
        sem_signal((sem_t)semGet);
    }

    char uGet()
    {
        sem_wait((sem_t)semGet);
        sem_wait((sem_t)mutex);
        char c = get();
        sem_signal((sem_t)mutex);
        sem_signal((sem_t)semPut);
        return c;
    }
*/
    void uPut(char c)
    {
        Kernel::mc_sstatus(Kernel::BitMaskSstatus::SSTATUS_SIE);
        semPut->wait();
        mutex->wait();
        put(c);
        mutex->signal();
        semGet->signal();
        Kernel::ms_sstatus(Kernel::BitMaskSstatus::SSTATUS_SIE);
    }

    char uGet()
    {
        Kernel::mc_sstatus(Kernel::BitMaskSstatus::SSTATUS_SIE);
        semGet->wait();
        mutex->wait();
        char c = get();
        mutex->signal();
        semPut->signal();
        Kernel::ms_sstatus(Kernel::BitMaskSstatus::SSTATUS_SIE);
        return c;
    }
    void* operator new   (size_t n) { return MemoryManager::kMemAlloc_byte(n); }
    void* operator new[] (size_t n) { return MemoryManager::kMemAlloc_byte(n); }
    void  operator delete   (void* p) noexcept { MemoryManager::kMemFree(p); }
    void  operator delete[] (void* p) noexcept { MemoryManager::kMemFree(p); }

private:

    void put(char c)
    {
        buffer[pos_w++] = c;
        pos_w %= length;
    }

    char get()
    {
        char c = buffer[pos_r++];
        pos_r %= length;
        return c;
    }

    static const size_t length = 256;
    char buffer[length];
    size_t pos_w;
    size_t pos_r;
    kSemaphore* semPut;
    kSemaphore* semGet;
    kSemaphore* mutex;

};

class IO
{

public:
    static void init();

    static CharBuffer* inputBuffer;
    static CharBuffer* outputBuffer;

    static bool inputPass;
    static bool outputPass;

//    static bool blockedAtInput() { return inputTCB->blockedAt() == inputSem; }
//    static bool blockedAtOutput() { return outputTCB->blockedAt() == outputSem; }

private:
    static void inputRoutine(void* p);
    static void outputRoutine(void* p);

//    kSemaphore* inputSem;
//    kSemaphore* outputSem;

    static TCB* inputTCB;
    static TCB* outputTCB;

    friend class Kernel;
};




#endif // __IO__HPP__