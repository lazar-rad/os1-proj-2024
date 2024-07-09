#include "../h/syscall_cpp.hpp"

#include "printing.hpp"

struct ThreadData11
{
    int num;
    Thread* threadA;
    Semaphore* parental;
};

static void threadA_11(void* p)
{
    ThreadData11* td = (ThreadData11*)p;

    Thread::sleep(10);

    printString("A finished\n");

    td->parental->signal();
}

static void threadB_11(void* p)
{
    ThreadData11* td = (ThreadData11*)p;

    int r = (td->num % 2 == 0) ? td->threadA->timedJoin(td->num) : td->threadA->join();

    if (r == 0)
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

        if (td->num == 4 || td->num == 6)
        {
            r = td->threadA->timedJoin(td->num);

            if (r == 0)
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
        }
    }

    td->parental->signal();
}

void testJoin()
{
    ThreadData11 threadData[20];
    Thread* threadB[20];

    Thread* threadA = new Thread(&threadA_11, &threadData[0]);
    Semaphore* parental = new Semaphore(0);

    for (int i = 0; i < 20; i++)
    {
        threadData[i].num = i+1;
        threadData[i].threadA = threadA;
        threadData[i].parental = parental;
        threadB[i] = new Thread(&threadB_11, &threadData[i]);
    }

    threadA->start();
    for (int i = 0; i < 20; i++)
        threadB[i]->start();

    for (int i = 0; i < 21; i++)
        parental->wait();
}
