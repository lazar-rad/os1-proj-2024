#ifndef __SCHEDULER__HPP__
#define __SCHEDULER__HPP__

#include "../lib/hw.h"

#include "../kerneltest/testers.hpp"

class TCB;

class Scheduler
{
    public:
        static void init();
        static void close() { while (readyHead) get(); }

        static void put(TCB* tcb);
        static TCB* get();

    private:
        static TCB* readyHead;
        static TCB* readyTail;
        static TCB* idle;
        static void spin(void* p);
        
#if USE_SCHEDULER_TESTER == 1
    friend class SchedulerTester;
#endif
};










#endif // __SCHEDULER__HPP__