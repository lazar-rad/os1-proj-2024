A small OS kernel for the RISC-V architecture
University project for the course “Operating systems 1” at ETF Belgrade

Written by Lazar Radosavljević
using project base and basic thread management principles provided by the lecturers

lecturers:
Dragan Milićev
Živojin Šuštran
Uroš Radenković
Marko Mićović
Luka Hrvačević
Predrag Obradović

The operating system provides the following services:
- thread management (create, exit, dispatch, timed join, join all)
- dynamic memory management (allocate, free)
- semaphores (wait, trywait, timedwait, signal)
- I/O operations
- messages

# Prerequisites

- C/C++ compiler collection for RISC-V (`riscv64-unknown-elf-gcc`, `riscv64-unknown-elf-g++`, or `riscv64-linux-gnu-gcc`, `riscv64-linux-gnu-g++`)
- QEMU emulator (https://www.qemu.org/docs/master/)
- multiarchitectural debugger (`gdb-multiarch`) (optional)

# How to use

The user should implement the method `void userMain()`. The headers `h/syscall_c.h`, `h/syscall_cpp.hpp`, and `test/printing.hpp` are meant for user’s use.
The file `test/userMain.cpp` serves for the purposes of testing and provides the method `void userMain()`, so when implementing their own `void userMain()`, the user should exclude that file from the compilation process (for example, by adding another extension to its name, so `make` doesn’t recognise it as a CPP source file).
The user can start an arbitrary number of threads. All user threads, including the one on `void userMain()`, run in the unprivileged mode.

# How it functions

Communication with the OS is done through system calls and has three layers: ABI, C API, and C++ API.
ABI is the lowest layer and it invokes system calls. When a system call is invoked, the OS expects the system call number in register `a0` and its arguments (if there are any) in registers `a1` through `a4`.
C API is a wrapper around ABI. C++ API is a wrapper around C API.
System call numbers are given in `h/syscall_c.h`.
In most cases, C API just passes its the arguments to the ABI. Exceptions are the following C API methods:
- `void* mem_alloc(size_t size)`: in the C API, `size` represents the size in bytes; in the ABI, it is the size in memory blocks (64 bytes)
- `int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg)`: ABI version is equivalent of `int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg, void* stack_space)`; the additional argument is the pointer to the bottom of the user-allocated stack; if `NULL` is passed, then the kernel is responsible for the allocation of the stack; C API defaults to the latter solution

# How it runs

The emulator launches one thread where `int main()` is called. In `int main()`, the initialisation of the system is done, system threads are created and started, and then the thread on `void userMain()` is started. After `void userMain()` finishes its execution, the system resources are freed and the emulator is terminated.

# Programming interfaces

## ABI

```C++
SYSCALL_MEM_ALLOC        0x01
SYSCALL_MEM_FREE         0x02
SYSCALL_THREAD_CREATE    0x11
SYSCALL_THREAD_EXIT      0x12
SYSCALL_THREAD_DISPATCH  0x13
SYSCALL_THREAD_TIMEDJOIN 0x14
SYSCALL_THREAD_JOINALL   0x15
SYSCALL_SEM_OPEN         0x21
SYSCALL_SEM_CLOSE        0x22
SYSCALL_SEM_WAIT         0x23
SYSCALL_SEM_SIGNAL       0x24
SYSCALL_SEM_TIMEDWAIT    0x25
SYSCALL_SEM_TRYWAIT      0x26
SYSCALL_TIME_SLEEP       0x31
SYSCALL_GETC             0x41
SYSCALL_PUTC             0x42
SYSCALL_SEND             0x51
SYSCALL_RECEIVE          0x52
```

## C API

```C
void* mem_alloc(size_t size);
int mem_free(void* p);
int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg);
int thread_exit();
void thread_dispatch();
int timedJoin(thread_t handle, time_t timeout);
void thread_joinall();
int sem_open(sem_t* handle, unsigned init);
int sem_close(sem_t handle);
int sem_wait(sem_t id);
int sem_signal(sem_t id);
int sem_timedwait(sem_t id, time_t timeout);
int sem_trywait(sem_t id);
int time_sleep(time_t timeout);
char getc();
void putc(char c);
void send(thread_t handle, const char* message);
const char* receive();
```

## C++ API

```C++
void* operator new (size_t n);
void  operator delete (void* p) noexcept;

class Thread
{
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t timeout);
    void send (const char* message);
    static const char* receive ();
    int timedJoin (time_t timeout);
    static void joinAll ();

protected:
    Thread ();
    virtual void run ();
};

class PeriodicThread : public Thread
{
public:
    void terminate ();

protected:
    PeriodicThread (time_t period);
    virtual void run () override;
    virtual void periodicActivation () {}
};

class Semaphore
{
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
    int timedWait (time_t timeout);
    int tryWait ();
};

class Console
{
public:
    static char getc ();
    static void putc (char);
};
```