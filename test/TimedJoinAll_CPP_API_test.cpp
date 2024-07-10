#include "../h/syscall_cpp.hpp"

#include "printing.hpp"
#include "../util/printingUtils.hpp"

static void threadA_12(void* p)
{
    int num = *((int*)p);

    Thread::sleep(num);

    log("Finished: ", num);
}

void testTimedJoinAll()
{
    const int numOfThreads = 40;

    int num[numOfThreads];
    Thread* threads[numOfThreads];
    int r;

    for (int i = 0; i < numOfThreads/2; i++)
    {
        num[i] = i+1;
        threads[i] = new Thread(&threadA_12, &num[i]);
        log("Starting thread ", num[i]);
        threads[i]->start();
    }

    r = Thread::timedJoinAll(0);
    log("Active children: ", r);

    log("TimeJoining all...");
    r = Thread::timedJoinAll(13);
    log("All joined :) ; remains ", r);

    log("TimeJoining all...");
    r = Thread::timedJoinAll(5);
    log("All joined :) ; remains ", r);

//    thread_exit();

    for (int i = numOfThreads/2; i < (numOfThreads/4)*3; i++)
    {
        num[i] = i+1;
        threads[i] = new Thread(&threadA_12, &num[i]);
        log("Starting thread ", num[i]);
        threads[i]->start();
    }

    Thread::sleep(27);

    log("TimeJoining all...");
    r = Thread::timedJoinAll(1);
    log("All joined :) ; remains ", r);
    
    for (int i = (numOfThreads/4)*3; i < numOfThreads; i++)
    {
        num[i] = i+1;
        threads[i] = new Thread(&threadA_12, &num[i]);
        log("Starting thread ", num[i]);
        threads[i]->start();
    }

    Thread::sleep(45);

    log("Joining all...");
    r = Thread::timedJoinAll(4);
    log("All joined :) ; remains ", r);

    for (int i = 0; i < numOfThreads; i++)
    {
        delete threads[i];
        threads[i] = nullptr;
    }
}
