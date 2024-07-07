#include "../h/syscall_c.h"
#include "../h/syscall_cpp.hpp"

// Memory management

void* operator new (size_t n)
{
    return mem_alloc(n);
}

void  operator delete (void* p) noexcept
{
    mem_free(p);
}


// Thread

void Thread::runWrapper(void* p)
{
    ((Thread*)p)->run();
}

int Thread::start()
{
    if (myHandle) return ERR_THREAD_STARTED;

    int ret;

    if (body)
        ret = thread_create(&myHandle, body, arg);
    else
        ret = thread_create(&myHandle, &Thread::runWrapper, this);

    return ret;
}

void PeriodicThread::run()
{
    while (period != stop)
    {
        periodicActivation();
        sleep(period);
    }
}


// Console

char Console::getc ()
{
    return ::getc();
}

void Console::putc (char c)
{
    ::putc(c);
}