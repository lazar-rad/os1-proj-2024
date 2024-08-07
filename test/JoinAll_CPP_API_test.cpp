#include "../h/syscall_cpp.hpp"

#include "printing.hpp"
#include "../util/mPrint.hpp"
#include "../util/mPrintUtils.hpp"

static void threadA_10(void* p)
{
    int num = *((int*)p);

    Thread::sleep(num);

    log("Finished: ", num);
}

void testJoinAll()
{
    const int numOfThreads = 20;

    int num[numOfThreads];
    Thread* threads[numOfThreads];

    for (int i = 0; i < numOfThreads; i++)
    {
        num[i] = i+1;
        threads[i] = new Thread(&threadA_10, &num[i]);
        log("Starting thread ", num[i]);
        threads[i]->start();
    }

    printString("Joining all...\n");
    Thread::joinAll();
    printString("All joined :)\n");

    for (int i = 0; i < numOfThreads; i++)
    {
        delete threads[i];
        threads[i] = nullptr;
    }
    
    for (int i = 0; i < numOfThreads/2; i++)
    {
        num[i] += numOfThreads;
        threads[i] = new Thread(&threadA_10, &num[i]);
        log("Starting thread ", num[i]);
        threads[i]->start();
    }

    Thread::sleep(27);

    printString("Joining all...\n");
    Thread::joinAll();
    printString("All joined :)\n");

    for (int i = 0; i < numOfThreads/2; i++)
    {
        delete threads[i];
        threads[i] = nullptr;
    }
    
    for (int i = numOfThreads/2; i < numOfThreads; i++)
    {
        num[i] += numOfThreads;
        threads[i] = new Thread(&threadA_10, &num[i]);
        log("Starting thread ", num[i]);
        threads[i]->start();
    }

    Thread::sleep(45);

    printString("Joining all...\n");
    Thread::joinAll();
    printString("All joined :)\n");

    for (int i = numOfThreads/2; i < numOfThreads; i++)
    {
        delete threads[i];
        threads[i] = nullptr;
    }
}
