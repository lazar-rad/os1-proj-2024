#include "../h/scheduler.hpp"
#include "../h/tcb.hpp"

TCB* Scheduler::readyHead = nullptr;
TCB* Scheduler::readyTail = nullptr;
TCB* Scheduler::idle = nullptr;

void Scheduler::init()
{
    idle = TCB::threadCreate(&spin, nullptr, TCB::Mode::SYSTEM, nullptr,
                             DEFAULT_TIME_SLICE, TCB::SchPut::NOPUT);
}

void Scheduler::put(TCB* tcb)
{
    if (!tcb || tcb == idle) return;

    tcb->nextReady = nullptr;

    if (!readyTail)
        readyHead = readyTail = tcb;
    else
        readyTail = readyTail->nextReady = tcb;
}

TCB* Scheduler::get()
{
    TCB* ret = nullptr;
    if (readyHead)
    {
        ret = readyHead;
        readyHead = readyHead->nextReady;
        if (!readyHead) readyTail = nullptr;
        ret->nextReady = nullptr;
    }
    else
    {
        ret = idle;
    }

    return ret;
}

void Scheduler::spin(void* p)
{
    (void)p;
}