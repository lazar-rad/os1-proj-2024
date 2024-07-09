#ifndef __TCB__HPP__
#define __TCB__HPP__

#include "../lib/hw.h"

#include "../h/memoryManager.hpp"
#include "../h/stack.hpp"
#include "../h/scheduler.hpp"

class kSemaphore;

using Body = void (*)(void*);

class TCB
{
public:

    enum Mode { USER = 0, SYSTEM };
    enum SchPut { PUT = true, NOPUT = false };

    ~TCB();

    static TCB* threadCreate(Body body, void* arg, Mode mode, void* userStackSpace, SchPut schPut = SchPut::PUT);

    static void threadDeleteAll();

    inline bool isFinished() const { return finished; }
    inline void setFinished(bool finished) { this->finished = finished; }
    inline uint64 getTimeSlice() const { return timeSlice; }
    inline uint64 getMode() const { return mode; }
    inline bool isSleeping() const { return sleeps; }
    inline kSemaphore* blockedAt() const { return blockedAtSem; }

    static TCB* running;

    inline static TCB* getTCBsHead() { return TCBsHead; }
    inline static TCB* getTCBsTail() { return TCBsTail; }

    static void yield(SchPut schPut = SchPut::PUT);

    void* operator new   (size_t n) { return MemoryManager::kMemAlloc_byte(n); }
    void* operator new[] (size_t n) { return MemoryManager::kMemAlloc_byte(n); }
    void  operator delete   (void* p) noexcept { MemoryManager::kMemFree(p); }
    void  operator delete[] (void* p) noexcept { MemoryManager::kMemFree(p); }

private:

    struct SPs
    {
        uint64 userSP;
        uint64 systemSP;
    };

    struct Context
    {
        uint64 ra;
        uint64 sp;
        uint64 s[12];
    };

    explicit TCB(Body body, void* arg, Mode mode, void* userStackSpace, uint64 timeSlice);

    uint64 threadID;
    Mode mode;
    TCB* parent;
    TCB* nextTCB;

    uint64 systemLevelCounter;
    StackField* userStack;
    StackField* systemStack;
    SPs sps;
    Context context;

    Body body;
    void* arg;

    bool finished;
    uint64 exitStatus;
    TCB* nextReady;

    kSemaphore* semTimedJoin;
    uint64 numOfJoining;

    kSemaphore* semJoinAll;
    uint64 numOfActiveChildren;

    uint64 timeSlice;
    bool sleeps;
    time_t timeSleepRelative;
    TCB* nextSleep;

    enum UnblockManner { REGULAR = 0, ON_CLOSE, WAIT_TIMEOUT };

    kSemaphore* blockedAtSem;
    TCB* nextSemBlocked;
    UnblockManner unblockManner;

    kSemaphore* semSend;
    kSemaphore* semReceive;
    static const size_t msgLen = 256;
    char message[msgLen+1];
    
    static uint64 IDCounter;
    static uint64 timeSliceCounter;

    static TCB* TCBsHead;
    static TCB* TCBsTail;
    
    static void threadWrapper();

    static void contextSwitch(Context* oldContext, Context* newContext);
    static void finish(uint64 exitStatus = 0);

    inline void setMsg(const char* msg)
    {
        size_t i;
        for (i = 0; msg[i] != '\0' && i < msgLen; i++)
            message[i] = msg[i];
        message[i] = '\0';
    }

    friend class Scheduler;
    friend class Kernel;
    friend class kSemaphore;
    friend class Sleep;

#if USE_TCB_TESTER == 1
    friend class TCBTester;
#endif
#if USE_SCHEDULER_TESTER == 1
    friend class SchedulerTester;
#endif
#if USE_SLEEP_TESTER == 1
    friend class SleepTester;
#endif
};



#endif // __TCB__HPP__