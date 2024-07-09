#ifndef _syscall_c
#define _syscall_c

#define SYSCALL_MEM_ALLOC        0x01
#define SYSCALL_MEM_FREE         0x02
#define SYSCALL_THREAD_CREATE    0x11
#define SYSCALL_THREAD_EXIT      0x12
#define SYSCALL_THREAD_DISPATCH  0x13
#define SYSCALL_THREAD_JOIN      0x14
#define SYSCALL_THREAD_TIMEDJOIN 0x15
#define SYSCALL_THREAD_JOINALL   0x16
//#define SYSCALL_THREAD_TIMEDJOINALL 0x17
#define SYSCALL_SEM_OPEN         0x21
#define SYSCALL_SEM_CLOSE        0x22
#define SYSCALL_SEM_WAIT         0x23
#define SYSCALL_SEM_SIGNAL       0x24
#define SYSCALL_SEM_TIMEDWAIT    0x25
#define SYSCALL_SEM_TRYWAIT      0x26
#define SYSCALL_TIME_SLEEP       0x31
#define SYSCALL_GETC             0x41
#define SYSCALL_PUTC             0x42
#define SYSCALL_SEND             0x51
#define SYSCALL_RECEIVE          0x52

#define ERR_MEM_FREE          -1
#define ERR_THREAD_CREATE     -1
#define ERR_THREAD_EXIT       -1
#define ERR_THREAD_STARTED    -2
#define ERR_THREAD_SELFJOIN   -3
#define ERR_SEM_DEAD          -1
#define ERR_SEM_CREATE        -3
#define ERR_SEM_WAITCLOSE     -4
#define ERR_SLEEP_ZERO        -1

#define SEMDEAD ERR_SEM_DEAD
#define TIMEOUT -2

#define RET_VOID(syscall_num) (syscall_num == SYSCALL_THREAD_DISPATCH || \
                               syscall_num == SYSCALL_THREAD_JOINALL || \
                               syscall_num == SYSCALL_PUTC || \
                               syscall_num == SYSCALL_SEND)

typedef unsigned char uint8;
typedef unsigned int uint;

typedef struct { } _thread;
typedef _thread* thread_t;

typedef struct { } _sem;
typedef _sem* sem_t;

typedef unsigned long size_t;
typedef unsigned long time_t;

const int EOF = -1;

#ifdef __cplusplus
extern "C" {
#endif


void* mem_alloc(size_t size); // syscall 0x01

int mem_free(void* p); // syscall 0x02

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg); // syscall 0x11

int thread_exit(); // syscall 0x12

void thread_dispatch(); // syscall 0x13

int thread_join(thread_t handle); // syscall 0x14

int thread_timedjoin(thread_t handle, time_t timeout); // syscall 0x15

void thread_joinall(); // syscall 0x16

int sem_open(sem_t* handle, unsigned init); // syscall 0x21

int sem_close(sem_t handle); // syscall 0x22

int sem_wait(sem_t id); // syscall 0x23

int sem_signal(sem_t id); // syscall 0x24

int sem_timedwait(sem_t id, time_t timeout); // syscall 0x25

int sem_trywait(sem_t id); // syscall 0x26

int time_sleep(time_t timeout); // syscall 0x31

char getc(); // syscall 0x41

void putc(char c); // syscall 0x42

void send(thread_t handle, const char* message); // syscall 0x51

const char* receive(); // syscall 0x52

#ifdef __cplusplus
}
#endif

#endif // _syscall_c