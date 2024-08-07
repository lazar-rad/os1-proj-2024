#include "../h/main.hpp"
#include "../h/syscall_c.h"
#include "../h/syscall_cpp.hpp"

#include "../h/memoryManager.hpp"
#include "../h/tcb.hpp"
#include "../h/kernel.hpp"
#include "../h/kSemaphore.hpp"
#include "../h/sleep.hpp"
#if USE_IO == 1
#include "../h/io.hpp"
#endif

#include "../util/mPrint.hpp"
#include "../util/mPrintUtils.hpp"

#include "../kerneltest/testers.hpp"
#if USE_MEMORY_MANAGER_TESTER == 1
#include "../kerneltest/memoryManagerTester.hpp"
#endif
#if USE_TCB_TESTER == 1
#include "../kerneltest/tcbTester.hpp"
#endif
#if USE_SCHEDULER_TESTER == 1
#include "../kerneltest/schedulerTester.hpp"
#endif
#if USE_KSEMAPHORE_TESTER == 1
#include "../kerneltest/kSemaphoreTester.hpp"
#endif
#if USE_SLEEP_TESTER == 1
#include "../kerneltest/sleepTester.hpp"
#endif
#if USE_MPRINT_TESTER == 1
#include "../kerneltest/mPrintTester.hpp"
#endif

#define USER_MAIN_WAIT_TIME 200

void __attribute__((weak)) userMain() { }

static void userMainWrapper(void* p)
{
    userMain();
#if USE_EMBEDDED_JOIN != 1
    ((Semaphore*)p)->signal();
#endif
    (void)p;
}

int main()
{

    // system initialization begin

    MemoryManager::init();

    Kernel::init();

    TCB::running = Kernel::master;

    Scheduler::init();

#if USE_IO == 1
    IO::init();
#endif

    Kernel::ms_sstatus(Kernel::BitMaskSstatus::SSTATUS_SIE);

    // system initialization end

    log("Kernel started");
#if USE_IO == 0
    log("using preprovided IO");
#endif
    endl;

#if USE_EMBEDDED_JOIN == 1
    thread_t userMainThread;
    thread_create(&userMainThread, &userMainWrapper, nullptr);
    thread_join(userMainThread);
#else
    Semaphore* userMainSem = new Semaphore(0);
    thread_t userMainThread;
    thread_create(&userMainThread, &userMainWrapper, userMainSem);
    while (userMainSem->timedWait(USER_MAIN_WAIT_TIME) == TIMEOUT)
    {
        if (((TCB*)userMainThread)->isFinished()) break;
    }
#endif

    // system closing begin

    endl;
    log("Kernel finished");
    Thread::sleep(5);
    
    Kernel::mc_sstatus(Kernel::BitMaskSstatus::SSTATUS_SIE);
    Kernel::mc_sip(Kernel::BitMaskSip::SIP_SSIP);
    kSemaphore::kSemaphoreDeleteAll();
    Sleep::wakeUpAll();
    Scheduler::close();
    TCB::threadDeleteAll();

    Kernel::terminate();

    // system closing end

    return 0;
}