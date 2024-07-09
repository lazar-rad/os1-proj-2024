#include "../h/memoryManager.hpp"
#include "../h/tcb.hpp"
#include "../h/kernel.hpp"
#include "../h/kSemaphore.hpp"
#include "../h/sleep.hpp"
#include "../h/io.hpp"

#include "../h/main.hpp"
#include "../h/syscall_c.h"
#include "../h/syscall_cpp.hpp"

#include "../test/printing.hpp"
#include "../util/printingUtils.hpp"

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

void __attribute__((weak)) userMain() { }

static void userMainWrapper(void* p)
{
    userMain();
    ((Semaphore*)p)->signal();
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

    Semaphore* userMainSem = new Semaphore(0);

    TCB* userMainThread;
    thread_create((thread_t*)&userMainThread, &userMainWrapper, userMainSem);

    while (userMainSem->timedWait(200) == TIMEOUT)
    {
        if (userMainThread->isFinished()) break;
    }

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