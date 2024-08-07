#include "../lib/hw.h"

#include "../h/tcb.hpp"
#include "../kerneltest/tcbTester.hpp"
#include "../kerneltest/testers.hpp"

#include "../util/mPrint.hpp"
#include "../util/mPrintUtils.hpp"

void TCBTester::probe(const TCB* tcb)
{
#if USE_TCB_TESTER == 1
    log("Probing TCB ", tcb->threadID);
    log("  this = ", tcb);
    log("  tcb->id = ", tcb->threadID);
    log("  tcb->mode = ", tcb->mode);
    log("  tcb->parent = ", tcb->parent);
    log("  tcb->sysLevel = ", tcb->systemLevelCounter);
    log("  tcb->userStack = ", tcb->userStack);
    log("  tcb->systemStack = ", tcb->systemStack);
    log("  tcb->sps.userSP = ", tcb->sps.userSP, 16);
    log("  tcb->sps.systemSP = ", tcb->sps.systemSP, 16);
    log("  tcb->context.ra = ", tcb->context.ra, 16);
    log("  tcb->context.sp = ", tcb->context.sp, 16);
    log("  tcb->body = ", (void*)tcb->body);
    log("  tcb->arg = ", tcb->arg);
    log("  tcb->finished = ", tcb->finished);
#endif
    (void)tcb;
}

void __attribute__((weak)) TCBTester::dummy (void* p) { (void)p; }

void __attribute__((weak)) TCBTester::test() { }