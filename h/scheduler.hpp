#ifndef __SCHEDULER__HPP__
#define __SCHEDULER__HPP__

#include "../lib/hw.h"

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
};










#endif // __SCHEDULER__HPP__