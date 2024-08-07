#include "../lib/hw.h"

#include "../h/scheduler.hpp"
#include "../kerneltest/schedulerTester.hpp"
#include "../kerneltest/testers.hpp"

#include "../util/mPrint.hpp"
#include "../util/mPrintUtils.hpp"

void SchedulerTester::probe()
{
#if USE_SCHEDULER_TESTER == 1
    mPrintString("Scheduler content:\n");

    for (TCB* tcb = Scheduler::readyHead; tcb; tcb = tcb->nextReady)
        log("  in Scheduler: ", tcb->threadID);
#endif
}

void __attribute__((weak)) SchedulerTester::test() { }