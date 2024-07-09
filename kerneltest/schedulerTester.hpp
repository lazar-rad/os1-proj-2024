#ifndef __SCHEDULER__TESTER__HPP__
#define __SCHEDULER__TESTER__HPP__

#include "../h/tcb.hpp"

#include "../kerneltest/testers.hpp"

class SchedulerTester
{
    public:
    static void probe();
    static void test();
#if USE_SCHEDULER_TESTER == 1
    inline static TCB* getReadyHead() { return Scheduler::readyHead; }
    inline static TCB* getReadyTail() { return Scheduler::readyTail; }
#endif
};


#endif // __SCHEDULER__TESTER__HPP__