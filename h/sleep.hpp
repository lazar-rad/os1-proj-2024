#ifndef __SLEEP__HPP__
#define __SLEEP__HPP__

#include "../h/tcb.hpp"
#include "../h/syscall_c.h"

class Sleep
{
public:
    static uint64 sleep(time_t timeout);
    static void onTick();

    static void wakeUpAll() { while (get()); }

private:
    static void put(TCB* tcb, time_t timeout);
    static TCB* get(TCB* tcb = nullptr);

    static TCB* sleepHead;

    friend class kSemaphore;
    friend void TCB::finish(uint64);
};

#endif // __SLEEP__HPP__