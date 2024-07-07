#include "../lib/hw.h"

#if USE_IO != 1
#include "../lib/console.h"
#endif

#include "../h/syscall_c.h"
#include "../h/syscall_cpp.hpp"

#include "../h/memoryManager.hpp"
#include "../h/kernel.hpp"
#include "../h/kSemaphore.hpp"
#include "../h/sleep.hpp"
#include "../h/io.hpp"

#include "../h/printingUtils.hpp"
#include "../test/printing.hpp"

bool Kernel::inInterrupt = false;

TCB* Kernel::master = nullptr;

InterruptRoutine Kernel::irr[irrNum] = { nullptr };

void userMainWrapper(void* p)
{
    userMain();
    ((Semaphore*)p)->signal();
}

/*
void userMain()
{

}
*/

void Kernel::init()
{
    w_stvec((uint64)&supervisorTrap);

    irr[SYSCALL_MEM_ALLOC] = &handleMemAlloc;
    irr[SYSCALL_MEM_FREE]  = &handleMemFree;
    irr[SYSCALL_THREAD_CREATE]    = &handleThreadCreate;
    irr[SYSCALL_THREAD_EXIT]      = &handleThreadExit;
    irr[SYSCALL_THREAD_DISPATCH]  = &handleThreadDispatch;
    irr[SYSCALL_THREAD_TIMEDJOIN] = &handleTimedJoin;
    irr[SYSCALL_THREAD_JOINALL]   = &handleJoinAll;
    irr[SYSCALL_SEM_OPEN]      = &handleSemOpen;
    irr[SYSCALL_SEM_CLOSE]     = &handleSemClose;
    irr[SYSCALL_SEM_WAIT]      = &handleSemWait;
    irr[SYSCALL_SEM_SIGNAL]    = &handleSemSignal;
    irr[SYSCALL_SEM_TIMEDWAIT] = &handleSemTimedWait;
    irr[SYSCALL_SEM_TRYWAIT]   = &handleSemTryWait;
    irr[SYSCALL_TIME_SLEEP] = &handleTimeSleep;
    irr[SYSCALL_GETC] = &handleGetc;
    irr[SYSCALL_PUTC] = &handlePutc;
    irr[SYSCALL_SEND] = &handleSend;
    irr[SYSCALL_RECEIVE] = &handleReceive;

    master = TCB::threadCreate(nullptr, nullptr, TCB::Mode::SYSTEM, nullptr);
}

void Kernel::handleSupervisorTrap(uint64 a0, uint64 a1, uint64 a2, uint64 a3, uint64 a4)
{
    Kernel::inInterrupt = true;
    TCB::running->systemLevelCounter++;
    volatile uint64 sepc = Kernel::r_sepc();
    volatile uint64 sstatus = Kernel::r_sstatus();
    volatile uint64 saved_a0 = Kernel::r_a0();
    volatile uint64 saved_a1 = Kernel::r_a1();

    uint64 scause = Kernel::r_scause();
    switch (scause)
    {
        case INTERRUPT_ECALL_USER:
        case INTERRUPT_ECALL_SYSTEM:
            if (a0 < irrNum && irr[a0])
            {
                uint64 ret = irr[a0](a1, a2, a3, a4);
                if (!RET_VOID(a0))
                    saved_a0 = ret;
            }
            sepc += 4;
            break;

        case INTERRUPT_TIMER:
            mc_sip(BitMaskSip::SIP_SSIP);
            TCB::timeSliceCounter++;
            Sleep::onTick();
            if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
            {
                TCB::yield();
            }
            break;
        case INTERRUPT_EXTERNAL:
#if USE_IO == 1
            if (plic_claim() == CONSOLE_IRQ)
            {
                plic_complete(CONSOLE_IRQ);
                IO::inputPass = 1;
                IO::outputPass = 1;
            }
#else
            console_handler();
#endif
            break;

        default:
            printString("# exception\n");
            log("  runningID: ",    TCB::running->threadID);
            log("  runningLevel: ", TCB::running->systemLevelCounter);
            log("  sepc = ",        sepc, 16);
            log("  sstatus = ",     Kernel::r_sstatus(), 16);
            log("  scause = ",      scause, 16);
            log("  stval = ",       Kernel::r_stval(), 16);
//            TCBTester::probe(TCB::running);
            Kernel::terminate();
            TCB::finish(-scause);
            break;
    }

    Kernel::w_a1(saved_a1);
    Kernel::w_a0(saved_a0);
    Kernel::w_sstatus(sstatus);
    Kernel::w_sepc(sepc);
    TCB::running->systemLevelCounter--;
    Kernel::inInterrupt = false;
}

uint64 Kernel::handleMemAlloc(uint64 a1_size, uint64 a2_0, uint64 a3_0, uint64 a4_0)
{
    return (uint64)MemoryManager::memAlloc(a1_size);
}

uint64 Kernel::handleMemFree(uint64 a1_p, uint64 a2_0, uint64 a3_0, uint64 a4_0)
{
    return MemoryManager::memFree((void*)a1_p);
}

uint64 Kernel::handleThreadCreate(uint64 a1_pHandle, uint64 a2_start_routine,
                                  uint64 a3_arg, uint64 a4_stack_space)
{
    TCB* tcb = TCB::threadCreate((Body)a2_start_routine, (void*)a3_arg,
                                 TCB::Mode::USER, (void*)a4_stack_space);
    *((TCB**)a1_pHandle) = tcb;
    return tcb ? 0 : ERR_THREAD_CREATE;
}

uint64 Kernel::handleThreadExit(uint64 a1_0, uint64 a2_0, uint64 a3_0, uint64 a4_0)
{
    TCB::finish();
    return ERR_THREAD_EXIT;
}

uint64 Kernel::handleThreadDispatch(uint64 a1_0, uint64 a2_0, uint64 a3_0, uint64 a4_0)
{
    TCB::yield(TCB::SchPut::PUT);
    return 0;
}

uint64 Kernel::handleTimedJoin(uint64 a1_handle, uint64 a2_timeout, uint64 a3_0, uint64 a4_0)
{
    TCB* tcb = (TCB*)a1_handle;
    if (tcb == TCB::running) return ERR_THREAD_SELFJOIN;
    if (tcb->isFinished()) return 0;
    tcb->numOfJoining++;
    int ret = tcb->semTimedJoin->timedWait((time_t)a2_timeout);
    if (ret == TIMEOUT && !tcb->isFinished()) tcb->numOfJoining--;
    return (uint64)ret;
}

uint64 Kernel::handleJoinAll(uint64 a1_0, uint64 a2_0, uint64 a3_0, uint64 a4_0)
{
    while (TCB::running->numOfActiveChildren > 0)
    {
        TCB::running->semJoinAll->wait();
        TCB::running->numOfActiveChildren--;
    }
    return 0;
}

uint64 Kernel::handleSemOpen(uint64 a1_pHandle, uint64 a2_init, uint64 a3_0, uint64 a4_0)
{
    kSemaphore* sem = kSemaphore::kSemaphoreCreate((uint64)a2_init);
    *((kSemaphore**)a1_pHandle) = sem;
    return sem ? 0 : ERR_SEM_CREATE;
}

uint64 Kernel::handleSemClose(uint64 a1_handle, uint64 a2_0, uint64 a3_0, uint64 a4_0)
{
    return (int)((kSemaphore*)a1_handle)->close();
}

uint64 Kernel::handleSemWait(uint64 a1_handle, uint64 a2_0, uint64 a3_0, uint64 a4_0)
{
    return (int)((kSemaphore*)a1_handle)->wait();
}

uint64 Kernel::handleSemSignal(uint64 a1_handle, uint64 a2_0, uint64 a3_0, uint64 a4_0)
{
    return (int)((kSemaphore*)a1_handle)->signal();
}

uint64 Kernel::handleSemTimedWait(uint64 a1_handle, uint64 a2_timeout, uint64 a3_0, uint64 a4_0)
{
    return (int)((kSemaphore*)a1_handle)->timedWait(a2_timeout);
}

uint64 Kernel::handleSemTryWait(uint64 a1_handle, uint64 a2_0, uint64 a3_0, uint64 a4_0)
{
    return (int)((kSemaphore*)a1_handle)->tryWait();
}

uint64 Kernel::handleTimeSleep(uint64 a1_timeout, uint64 a2_0, uint64 a3_0, uint64 a4_0)
{
    return (uint64)Sleep::sleep((time_t)a1_timeout);
}

uint64 Kernel::handleGetc(uint64 a1_0, uint64 a2_0, uint64 a3_0, uint64 a4_0)
{
#if USE_IO == 1
    return IO::inputBuffer->kGet();
#else
    return (uint64)__getc();
#endif
}

uint64 Kernel::handlePutc(uint64 a1_c, uint64 a2_0, uint64 a3_0, uint64 a4_0)
{
#if USE_IO == 1
    IO::outputBuffer->kPut((char)a1_c);
#else
    __putc((char)a1_c);
#endif
    return 0;
}

uint64 Kernel::handleSend(uint64 a1_handle, uint64 a2_msg, uint64 a3_0, uint64 a4_0)
{
    TCB* tcb = (TCB*)a1_handle;
    if (tcb == TCB::running) return 0;
    tcb->semSend->wait();
    tcb->setMsg((const char*)a2_msg);
    tcb->semReceive->signal();
    return 0;
}

uint64 Kernel::handleReceive(uint64 a1_0, uint64 a2_0, uint64 a3_0, uint64 a4_0)
{
    TCB::running->semReceive->wait();
    char* msg = TCB::running->message;
    TCB::running->semSend->signal();
    return (uint64)msg;
}