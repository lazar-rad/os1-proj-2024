#include "../lib/hw.h"

#include "../h/scheduler.hpp"
#include "../kerneltest/schedulerTester.hpp"

#include "../test/printing.hpp"
#include "../util/printingUtils.hpp"

void SchedulerTester::probe()
{
#if USE_SCHEDULER_TESTER == 1
    printString("Scheduler content:\n");

    for (TCB* tcb = Scheduler::readyHead; tcb; tcb = tcb->nextReady)
        log("    in Scheduler: ", tcb->threadID);
#endif
}

void __attribute__((weak)) SchedulerTester::test() { }