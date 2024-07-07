#ifndef _syscall_cpp
#define _syscall_cpp

#include "../h/syscall_c.h"

void* operator new (size_t n);

void  operator delete (void* p) noexcept;


class Thread
{
public:
    Thread (void (*body)(void*), void* arg) : myHandle(nullptr), body(body), arg(arg) { }

    virtual ~Thread () { }

    int start ();

    static void dispatch () { thread_dispatch(); }

    static int sleep (time_t timeout) { return time_sleep(timeout); }

    void send(const char* message) { ::send(myHandle, message); }

    static const char* receive() { return ::receive(); }

    int timedJoin(time_t timeout) { return ::timedJoin(myHandle, timeout); }

    static void joinAll() { return thread_joinall(); }

protected:
    Thread () : myHandle(nullptr), body(nullptr), arg(nullptr) { }
    virtual void run () {}

private:
    static void runWrapper(void* p);

    thread_t myHandle;
    void (*body)(void*);
    void* arg;
};


class PeriodicThread : public Thread
{
public:
    void terminate ();

protected:
    PeriodicThread (time_t period) : period(period) { }
    virtual void run() override;
    virtual void periodicActivation () {}

private:
    time_t period;
    static const time_t stop = ~0UL;
};


class Semaphore
{
public:
    Semaphore(unsigned init = 1) { sem_open(&myHandle, init); }

    virtual ~Semaphore() { sem_close(myHandle); }

    int wait()           { return sem_wait(myHandle); }

    int signal()         { return sem_signal(myHandle); }

    int timedWait (time_t timeout) { return sem_timedwait(myHandle, timeout); }

    int tryWait()        { return sem_trywait(myHandle); }

private:
    sem_t myHandle;
};


class Console
{
public:
    static char getc ();
    static void putc (char);
};

#endif // syscall_cpp
