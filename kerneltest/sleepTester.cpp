#include "../lib/hw.h"

#include "../h/sleep.hpp"
#include "../kerneltest/sleepTester.hpp"
#include "../kerneltest/testers.hpp"

#include "../h/tcb.hpp"

#include "../util/mPrint.hpp"
#include "../util/mPrintUtils.hpp"

void SleepTester::probe()
{
#if USE_SLEEP_TESTER == 1
    mPrintString("Sleep probe:\n");
    TCB* tcb = Sleep::sleepHead;
    while (tcb)
    {
        mPrintString("  ");
        mPrintInt(tcb->threadID);
        mPrintString(" ");
        mPrintInt(tcb->timeSleepRelative);
        if (tcb->blockedAtSem) mPrintString(" s");
        endl;
        tcb = tcb->nextSleep;
    }
#endif
}

void __attribute__((weak)) SleepTester::test(int n) { (void)n; }