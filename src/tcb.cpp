#include "../h/kernel.hpp"
#include "../h/tcb.hpp"
#include "../h/scheduler.hpp"
#include "../h/kSemaphore.hpp"
#include "../h/sleep.hpp"

#include "../h/syscall_c.h"

#include "../test/printing.hpp"
#include "../h/printingUtils.hpp"

TCB* TCB::running = nullptr;

uint64 TCB::IDCounter = 0;
uint64 TCB::timeSliceCounter = 0;

TCB* TCB::TCBsHead;
TCB* TCB::TCBsTail;

TCB* TCB::threadCreate(Body body, void* arg, Mode mode, void* userStackSpace, SchPut schPut)
{
    TCB* tcb = new TCB(body, arg, mode, userStackSpace, DEFAULT_TIME_SLICE);
    if (tcb->isFinished())
    {
        delete tcb;
        tcb = nullptr;
    }
    else
    {
        if (!TCBsTail)
            TCBsHead = TCBsTail = tcb;
        else
            TCBsTail = TCBsTail->nextTCB = tcb;

        if (body && schPut) Scheduler::put(tcb);
    }
    return tcb;
}

void TCB::threadDeleteAll()
{
    while (TCBsHead)
    {
        TCB* del = TCBsHead;
        TCBsHead = del->nextTCB;
        if (!TCBsHead) TCBsTail = nullptr;
        delete del;
    }
}

void TCB::yield(TCB::SchPut schPut)
{
    TCB* old = running;
    if (!old->isFinished() &&
        !old->blockedAtSem &&
        !old->sleeps &&
        schPut)
        Scheduler::put(old);
    running = Scheduler::get();
    timeSliceCounter = 0;
    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    if (running->mode == Mode::USER)
        Kernel::mc_sstatus(Kernel::BitMaskSstatus::SSTATUS_SPP);
    else
        Kernel::ms_sstatus(Kernel::BitMaskSstatus::SSTATUS_SPP);
    Kernel::inInterrupt = false;
    Kernel::popSppSpie();
    running->body(running->arg);
    thread_exit();
}

void TCB::finish(uint64 exitStatus)
{
    if (TCB::running->blockedAtSem) TCB::running->blockedAtSem->get(TCB::running);
    if (TCB::running->sleeps) Sleep::get(TCB::running);
    TCB::running->exitStatus = exitStatus;
    TCB::running->setFinished(true);
    TCB::yield(TCB::SchPut::NOPUT);
}