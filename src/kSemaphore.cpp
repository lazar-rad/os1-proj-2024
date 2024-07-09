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

uint64 kSemaphore::getBlockedCount()
{
    uint64 ret = 0;
    for (TCB* it = blockedHead; it; it = it->nextSemBlocked) ret++;
    return ret;
}

uint64 kSemaphore::getTokensNeeded()
{
    uint64 ret = 0;
    for (TCB* it = blockedHead; it; it = it->nextSemBlocked) ret += it->tokensNeeded;
    return ret;
}

uint64 kSemaphore::close()
{
    if (!active) return ERR_SEM_DEAD;
    signalAll(0, true);
    active = false;
    return 0;
}

void kSemaphore::put(TCB* tcb, uint64 tokens)
{
    if (!tcb) return;

    if (!blockedHead)
        blockedHead = blockedTail = tcb;
    else
        blockedTail = blockedTail->nextSemBlocked = tcb;

    tcb->blockedAtSem = this;
    tcb->nextSemBlocked = nullptr;
    tcb->tokensNeeded = tokens;
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
        ret->tokensNeeded = 0;
    }

    return ret;
}

uint64 kSemaphore::wait(uint64 tokens)
{
    if (!active) return ERR_SEM_DEAD;

    if (value >= tokens)
    {
        value -= tokens;
        TCB::running->unblockManner = TCB::UnblockManner::REGULAR;
    }
    else
    {
        tokens -= value;
        value = 0;
        put(TCB::running, tokens);
        TCB::yield(TCB::SchPut::NOPUT);
    }

    return resolveManner();
}

uint64 kSemaphore::signal(uint64 tokens)
{
    if (!active) return ERR_SEM_DEAD;

    while (blockedHead && tokens > 0)
    {
        if (tokens >= blockedHead->tokensNeeded)
        {
            tokens -= blockedHead->tokensNeeded;
            blockedHead->tokensNeeded = 0;
            TCB* tcb = get();
            if (tcb->sleeps) Sleep::get(tcb);
            tcb->unblockManner = TCB::UnblockManner::REGULAR;
            Scheduler::put(tcb);
        }
        else
        {
            blockedHead->tokensNeeded -= tokens;
            tokens = 0;
        }
    }

    value += tokens;

    return 0;
}

uint64 kSemaphore::signalAll(uint64 tokens, bool onClose)
{
    if (!active) return ERR_SEM_DEAD;

    while (blockedHead)
    {
        TCB* tcb = get();
        if (tcb->sleeps) Sleep::get(tcb);
        tcb->unblockManner = onClose ? TCB::UnblockManner::ON_CLOSE
                                     : TCB::UnblockManner::REGULAR;
        Scheduler::put(tcb);
    }

    value = tokens;

    return 1;
}

uint64 kSemaphore::tryWait(uint64 tokens)
{
    if (!active) return ERR_SEM_DEAD;

    uint64 ret = 0;

    if (value >= tokens)
        value -= tokens, ret = 0;
    else
        ret = 1;

    TCB::running->unblockManner = TCB::UnblockManner::REGULAR;

    return ret;
}

uint64 kSemaphore::timedWait(time_t timeout, uint64 tokens)
{
    if (!active) return ERR_SEM_DEAD;

    if (value >= tokens)
    {
        value -= tokens;
        TCB::running->unblockManner = TCB::UnblockManner::REGULAR;
    }
    else
    {
        tokens -= value;
        value = 0;
        put(TCB::running, tokens);
        Sleep::sleep(timeout);
    }

    return resolveManner();
}