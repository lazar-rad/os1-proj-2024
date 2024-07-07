#include "../h/syscall_cpp.hpp"

#include "printing.hpp"

struct ThreadData
{
    Thread* threads[3];
    Semaphore* sems[3];
};

static void threadA(void* p)
{
    ThreadData* td = (ThreadData*)p;

    for (int i = 0; i < 5; i++)
    {
        td->threads[1]->send("Nit A -> Nit B");
        td->threads[2]->send("Nit A -> Nit C");

        const char* msg;

        msg = Thread::receive();
        printString(msg);
        printString("\n");
    }

    td->sems[0]->signal();
}

static void threadB(void* p)
{
    ThreadData* td = (ThreadData*)p;

    for (int i = 0; i < 5; i++)
    {
        td->threads[2]->send("Nit B -> Nit C #1");
        td->threads[2]->send("Nit B -> Nit C #2");

        const char* msg;

        msg = Thread::receive();
        printString(msg);
        printString("\n");

        msg = Thread::receive();
        printString(msg);
        printString("\n");
    }

    td->sems[1]->signal();
}

static void threadC(void* p)
{
    ThreadData* td = (ThreadData*)p;

    for (int i = 0; i < 5; i++)
    {

        const char* msg;

        msg = Thread::receive();
        printString(msg);
        printString("\n");

        msg = Thread::receive();
        printString(msg);
        printString("\n");

        td->threads[0]->send("Nit C -> Nit A");
    }

    td->sems[2]->signal();
}

void testSendReceive()
{
    ThreadData threadData;
    threadData.sems[0] = new Semaphore(0);
    threadData.sems[1] = new Semaphore(1);
    threadData.sems[2] = new Semaphore(2);
    threadData.threads[0] = new Thread(&threadA, &threadData);
    threadData.threads[1] = new Thread(&threadB, &threadData);
    threadData.threads[2] = new Thread(&threadC, &threadData);

    threadData.threads[0]->start();
    threadData.threads[1]->start();
    threadData.threads[2]->start();

    threadData.sems[0]->wait();
    threadData.sems[1]->wait();
    threadData.sems[2]->wait();
}
