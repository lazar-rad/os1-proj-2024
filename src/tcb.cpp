#include "../h/kernel.hpp"
#include "../h/tcb.hpp"
#include "../h/scheduler.hpp"
#include "../h/kSemaphore.hpp"
#include "../h/sleep.hpp"

#include "../h/syscall_c.h"

#include "../test/printing.hpp"
#include "../util/printingUtils.hpp"

TCB* TCB::running = nullptr;

uint64 TCB::IDCounter = 0;
uint64 TCB::timeSliceCounter = 0;

TCB* TCB::TCBsHead;
TCB* TCB::TCBsTail;

TCB::TCB(Body body, void* arg, Mode mode, void* userStackSpace, uint64 timeSlice) :
        threadID(IDCounter++), mode(mode), parent(running), nextTCB(nullptr),
        systemLevelCounter(body ? 0 : 1),
        userStack(body && !userStackSpace ? new StackField[DEFAULT_STACK_SIZE] : nullptr),
        systemStack(body ? new StackField[DEFAULT_STACK_SIZE] : nullptr),
        sps(
            {
                userStackSpace ? (uint64)userStackSpace : (userStack ? (uint64) &userStack[DEFAULT_STACK_SIZE] : 0),
                systemStack ? (uint64) &systemStack[DEFAULT_STACK_SIZE] : 0
            }
        ),
        context(
            {
                (uint64)&threadWrapper,
                sps.userSP,
                { 0 }
            }
        ),
        body(body), arg(arg),
        finished(false), exitStatus(0), nextReady(nullptr),
        semJoin(kSemaphore::kSemaphoreCreate(0)),
        semJoinAll(kSemaphore::kSemaphoreCreate(0)), numOfActiveChildren(0),
        timeSlice(timeSlice == 0 ? 1 : (timeSlice > maxTimeSlice ? maxTimeSlice : timeSlice)),
        sleeps(false), timeSleepRelative(0), nextSleep(nullptr),
        tokensNeeded(0), blockedAtSem(nullptr), nextSemBlocked(nullptr),
        unblockManner(UnblockManner::REGULAR),
        semSend(kSemaphore::kSemaphoreCreate(1)), semReceive(kSemaphore::kSemaphoreCreate(0))
    {
        if ((body && (sps.userSP == 0 || sps.systemSP == 0))
            || !semSend || !semReceive || !semJoin || !semJoinAll)
        {
            delete[] userStack;
            userStack = nullptr;
            delete[] systemStack;
            systemStack = nullptr;
            finished = true;
            if (semSend) semSend->close();
            if (semReceive) semReceive->close();
            if (semJoin) semJoin->close();
            if (semJoinAll) semJoinAll->close();
        }
        else
        {
            for (size_t i = 0; i <= msgLen; i++)
                message[msgLen] = '\0';
        }
    }

TCB::~TCB()
{
    delete userStack;
    delete systemStack;
    nextTCB = nullptr;
    nextReady = nullptr;
    nextSleep = nullptr;
    nextSemBlocked = nullptr;
    if (semSend) semSend->close();
    if (semReceive) semReceive->close();
    if (semJoin) semJoin->close();
    if (semJoinAll) semJoinAll->close();
}

TCB* TCB::threadCreate(Body body, void* arg, Mode mode, void* userStackSpace,
                       uint64 timeSlice, SchPut schPut)
{
    TCB* tcb = new TCB(body, arg, mode, userStackSpace, timeSlice);
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
        if (tcb->parent) tcb->parent->numOfActiveChildren++;
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
    TCB::running->semSend->close();
    TCB::running->semReceive->close();

    TCB::running->semJoin->signalAll();
    TCB::running->semJoin->close();

    if (TCB::running->parent && !TCB::running->parent->isFinished())
    {
        TCB::running->parent->numOfActiveChildren--;
        if (TCB::running->parent->blockedAtSem == TCB::running->parent->semJoinAll)
            TCB::running->parent->semJoinAll->signal();    
    }
    TCB::running->semJoinAll->close();

    if (TCB::running->blockedAtSem) TCB::running->blockedAtSem->get(TCB::running);
    if (TCB::running->sleeps) Sleep::get(TCB::running);

    TCB::running->exitStatus = exitStatus;
    TCB::running->setFinished(true);
    TCB::yield(TCB::SchPut::NOPUT);
}

uint64 TCB::numOfJoining()
{
    return semJoin->getBlockedCount();
}