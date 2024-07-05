#include "../h/sleep.hpp"
#include "../h/kSemaphore.hpp"

TCB* Sleep::sleepHead = nullptr;

void Sleep::put(TCB* tcb, time_t timeout)
{
    if (!tcb) return;

    if (!sleepHead)
    {
        tcb->timeSleepRelative = timeout;
        tcb->nextSleep = nullptr;
        sleepHead = tcb;
    }
    else
    {
        if(timeout < sleepHead->timeSleepRelative)
        {
            sleepHead->timeSleepRelative -= timeout;
            tcb->timeSleepRelative = timeout;
            tcb->nextSleep = sleepHead;
            sleepHead = tcb;
        }
        else
        {
            TCB* target = sleepHead;
            timeout -= target->timeSleepRelative;
            while (target->nextSleep && target->nextSleep->timeSleepRelative <= timeout)
            {
                target = target->nextSleep;
                timeout -= target->timeSleepRelative;
            }
            if (target->nextSleep) target->nextSleep->timeSleepRelative -= timeout;
            tcb->timeSleepRelative = timeout;
            tcb->nextSleep = target->nextSleep;
            target->nextSleep = tcb;
        }
    }

    tcb->sleeps = true;
}

TCB* Sleep::get(TCB* tcb)
{
    TCB* ret = nullptr;

    if (tcb)
    {
        if (tcb->sleeps)
        {
            TCB* curr = sleepHead;
            TCB* prev = nullptr;
            while (curr && curr != tcb) prev = curr, curr = curr->nextSleep;
            if (curr)
            {
                ret = curr;
                if (prev)
                {
                    prev->nextSleep = curr->nextSleep;
                }
                else
                {
                    sleepHead = curr->nextSleep;
                }
                if (ret->nextSleep) ret->nextSleep->timeSleepRelative += ret->timeSleepRelative;
            }
        }

    }
    else
    {
        if (sleepHead)
        {
            ret = sleepHead;
            sleepHead = sleepHead->nextSleep;
            if (sleepHead) sleepHead->timeSleepRelative += ret->timeSleepRelative;
        }
    }

    if (ret)
    {
        ret->timeSleepRelative = 0;
        ret->sleeps = false;
        ret->nextSleep = nullptr;
    }

    return ret;
}

uint64 Sleep::sleep(time_t timeout)
{
    if (timeout == 0)
    {
        if (TCB::running->blockedAtSem)
        {
            TCB::running->blockedAtSem->get(TCB::running);
            TCB::running->unblockManner = TCB::UnblockManner::WAIT_TIMEOUT;
        }
        return ERR_SLEEP_ZERO;
    }

    put(TCB::running, timeout);

    TCB::yield(TCB::SchPut::NOPUT);

    return 0;
}

void Sleep::onTick()
{
    if (sleepHead) sleepHead->timeSleepRelative--;

    while (sleepHead && sleepHead->timeSleepRelative == 0)
    {
        TCB* tcb = get();
        if (tcb->blockedAtSem)
        {
            tcb->blockedAtSem->get(tcb);
            tcb->unblockManner = TCB::UnblockManner::WAIT_TIMEOUT;
        }
        Scheduler::put(tcb);
    }
}