#include "../lib/hw.h"

#include "../h/syscall_c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ROUNDUP(size, block) ((size+block-1)/block)

uint64 syscall(uint64 a0, uint64 a1, uint64 a2, uint64 a3, uint64 a4);

void* mem_alloc(size_t size)
{
    return (void*)syscall(SYSCALL_MEM_ALLOC, ROUNDUP(size, MEM_BLOCK_SIZE), 0, 0, 0);
}

int mem_free(void* p)
{
    if (!p) return 0;
    return (int)syscall(SYSCALL_MEM_FREE, (uint64)p, 0, 0, 0);
}

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg)
{
    return (int)syscall(SYSCALL_THREAD_CREATE, (uint64)handle,
                        (uint64)start_routine, (uint64)arg, 0);
}

int thread_exit()
{
    return (int)syscall(SYSCALL_THREAD_EXIT, 0, 0, 0, 0);
}

void thread_dispatch()
{
    syscall(SYSCALL_THREAD_DISPATCH, 0, 0, 0, 0);
}

int thread_timedjoin(thread_t handle, time_t timeout)
{
    return (int)syscall(SYSCALL_THREAD_TIMEDJOIN, (uint64)handle, (uint64)timeout, 0, 0);
}

void thread_joinall()
{
    syscall(SYSCALL_THREAD_JOINALL, 0, 0, 0, 0);
}

int sem_open(sem_t* handle, unsigned init)
{
    return (int)syscall(SYSCALL_SEM_OPEN, (uint64)handle, (uint64)init, 0, 0);
}

int sem_close(sem_t handle)
{
    return (int)syscall(SYSCALL_SEM_CLOSE, (uint64)handle, 0, 0, 0);
}

int sem_wait(sem_t id)
{
    return (int)syscall(SYSCALL_SEM_WAIT, (uint64)id, 0, 0, 0);
}

int sem_signal(sem_t id)
{
    return (int)syscall(SYSCALL_SEM_SIGNAL, (uint64)id, 0, 0, 0);
}

int sem_timedwait(sem_t id, time_t timeout)
{
    return (int)syscall(SYSCALL_SEM_TIMEDWAIT, (uint64)id, (uint64)timeout, 0, 0);
}

int sem_trywait(sem_t id)
{
    return (int)syscall(SYSCALL_SEM_TRYWAIT, (uint64)id, 0, 0, 0);
}

int time_sleep(time_t timeout)
{
    return (int)syscall(SYSCALL_TIME_SLEEP, timeout, 0, 0, 0);
}

char getc()
{
    return (char)syscall(SYSCALL_GETC, 0, 0, 0, 0);
}

void putc(char c)
{
    syscall(SYSCALL_PUTC, (uint64)c, 0, 0, 0);
}

void send(thread_t handle, const char* message)
{
    syscall(SYSCALL_SEND, (uint64)handle, (uint64)message, 0, 0);
}

const char* receive()
{
    return (const char*)syscall(SYSCALL_RECEIVE, 0, 0, 0, 0);
}

#ifdef __cplusplus
}
#endif