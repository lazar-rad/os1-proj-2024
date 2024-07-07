#include "../h/memoryManager.hpp"
#include "../h/tcb.hpp"
#include "../h/kernel.hpp"
#include "../h/kSemaphore.hpp"
#include "../h/sleep.hpp"
#include "../h/io.hpp"

#include "../h/syscall_c.h"
#include "../h/syscall_cpp.hpp"

#include "../test/printing.hpp"

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

    Semaphore* userMainSem = new Semaphore(0);

    TCB* userMainThread;
    thread_create((thread_t*)&userMainThread, &userMainWrapper, userMainSem);

    while (userMainSem->timedWait(200) == TIMEOUT)
    {
        if (userMainThread->isFinished()) break;
    }

    // system closing begin

    printString("Kernel finished\n");
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