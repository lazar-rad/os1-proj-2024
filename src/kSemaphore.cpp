#include "../h/kSemaphore.hpp"
#include "../h/scheduler.hpp"
#include "../h/sleep.hpp"

#include "../h/syscall_c.h"

kSemaphore* kSemaphore::kSemsHead = nullptr;
kSemaphore* kSemaphore::kSemsTail = nullptr;

uint64 kSemaphore::IDCounter = 0;

kSemaphore* kSemaphore::kSemaphoreCreate(uint64 val)
{
    kSemaphore* sem = new kSemaphore(val);

    if (!sem) return nullptr;

    if (!kSemsTail)
        kSemsHead = kSemsTail = sem;
    else
        kSemsTail = kSemsTail->nextSem = sem;

    return sem;
}

void kSemaphore::kSemaphoreDeleteAll()
{
    while (kSemsHead)
    {
        kSemaphore* del = kSemsHead;
        kSemsHead = del->nextSem;
        if (!kSemsHead) kSemsTail = nullptr;
        if (del->active) del->close();
        delete del;
    }
}

uint64 kSemaphore::close()
{
    if (!active) return ERR_SEM_DEAD;
    while (blockedHead) signal(true);
    active = false;
    return 0;
}

void kSemaphore::put(TCB* tcb)
{
    if (!tcb) return;

    if (!blockedHead)
        blockedHead = blockedTail = tcb;
    else
        blockedTail = blockedTail->nextSemBlocked = tcb;

    tcb->blockedAtSem = this;
    tcb->nextSemBlocked = nullptr;
}

TCB* kSemaphore::get(TCB* tcb)
{
    TCB* ret = nullptr;

    if (tcb)
    {
        if (tcb->blockedAtSem == this)
        {
            TCB* curr = blockedHead;
            TCB* prev = nullptr;
            while (curr && curr != tcb) prev = curr, curr = curr->nextSemBlocked;
            if (curr)
            {
                ret = curr;
                if (prev)
                {
                    prev->nextSemBlocked = curr->nextSemBlocked;
                    if (curr == blockedTail) blockedTail = prev;
                }
                else
                {
                    blockedHead = curr->nextSemBlocked;
                    if (!blockedHead) blockedTail = nullptr;
                }
            }
        }
    }
    else
    {
        if (blockedHead)
        {
            ret = blockedHead;
            blockedHead = blockedHead->nextSemBlocked;
            if (!blockedHead) blockedTail = nullptr;
        }
    }

    if (ret)
    {
        ret->blockedAtSem = nullptr;
        ret->nextSemBlocked = nullptr;
    }

    return ret;
}

uint64 kSemaphore::wait()
{
    if (!active) return ERR_SEM_DEAD;

    if (value > 0)
    {
        value--;
        TCB::running->unblockManner = TCB::UnblockManner::REGULAR;
    }
    else
    {
        put(TCB::running);
        TCB::yield(TCB::SchPut::NOPUT);
    }

    return resolveManner();
}

uint64 kSemaphore::signal(bool onClose)
{
    if (!active) return ERR_SEM_DEAD;

    if (!blockedHead)
        value++;
    else
    {
        TCB* tcb = get();
        if (tcb->sleeps) Sleep::get(tcb);
        tcb->unblockManner = onClose ? TCB::UnblockManner::ON_CLOSE
                                     : TCB::UnblockManner::REGULAR;
        Scheduler::put(tcb);
    }

    return 0;
}

uint64 kSemaphore::tryWait()
{
    if (!active) return ERR_SEM_DEAD;

    uint64 ret = 0;

    if (value > 0)
        value--, ret = 0;
    else
        ret = 1;

    TCB::running->unblockManner = TCB::UnblockManner::REGULAR;

    return ret;
}

uint64 kSemaphore::timedWait(time_t timeout)
{
    if (!active) return ERR_SEM_DEAD;

    if (value > 0)
    {
        value--;
        TCB::running->unblockManner = TCB::UnblockManner::REGULAR;
    }
    else
    {
        put(TCB::running);
        Sleep::sleep(timeout);
    }

    return resolveManner();
}