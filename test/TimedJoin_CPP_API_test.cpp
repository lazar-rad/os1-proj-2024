#include "../h/syscall_cpp.hpp"

#include "printing.hpp"

struct ThreadData9
{
    int num;
    Thread* threadA;
    Semaphore* parental;
};

static void threadA_9(void* p)
{
    ThreadData9* td = (ThreadData9*)p;

    Thread::sleep(10);

    printString("A finished\n");

    td->parental->signal();
}

static void threadB_9(void* p)
{
    ThreadData9* td = (ThreadData9*)p;

    if (td->threadA->timedJoin(td->num) == 0)
    {
        printString("Nit B broj ");
        printInt(td->num);
        printString(" je uspesno sacekala nit A\n");
    }
    else
    {
        printString("Nit B broj ");
        printInt(td->num);
        printString(" je neuspesno sacekala nit A\n");
    }

    td->parental->signal();
}

void testTimedJoin()
{
    ThreadData9 threadData[20];
    Thread* threadB[20];

    Thread* threadA = new Thread(&threadA_9, &threadData[0]);
    Semaphore* parental = new Semaphore(0);

    for (int i = 0; i < 20; i++)
    {
        threadData[i].num = i+1;
        threadData[i].threadA = threadA;
        threadData[i].parental = parental;
        threadB[i] = new Thread(&threadB_9, &threadData[i]);
    }

    threadA->start();
    for (int i = 0; i < 20; i++)
        threadB[i]->start();

    for (int i = 0; i < 21; i++)
        parental->wait();
}
