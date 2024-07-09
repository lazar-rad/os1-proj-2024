#include "../lib/hw.h"

#include "../h/sleep.hpp"
#include "../kerneltest/sleepTester.hpp"
#include "../kerneltest/testers.hpp"

#include "../h/tcb.hpp"

#include "../test/printing.hpp"
#include "../util/printingUtils.hpp"

void SleepTester::probe()
{
#if USE_SLEEP_TESTER == 1
    printString("Sleep probe:\n");
    TCB* tcb = Sleep::sleepHead;
    while (tcb)
    {
        printString("  ");
        printInt(tcb->threadID);
        printString(" ");
        printInt(tcb->timeSleepRelative);
        if (tcb->blockedAtSem) printString(" s");
        endl;
        tcb = tcb->nextSleep;
    }
#endif
}

void __attribute__((weak)) SleepTester::test(int n) { while (n && !n); }