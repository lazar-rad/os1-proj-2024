#ifndef __KERNEL__HPP__
#define __KERNEL__HPP__

#include "../lib/hw.h"

#include "../h/tcb.hpp"

#define INTERRUPT_TIMER        ((0x01UL<<63)|(0x01UL))
#define INTERRUPT_EXTERNAL     ((0x01UL<<63)|(0x09UL))
#define INTERRUPT_EXC_INSTR    ((0x00UL<<63)|(0x02UL))
#define INTERRUPT_EXC_READ     ((0x00UL<<63)|(0x05UL))
#define INTERRUPT_EXC_WRITE    ((0x00UL<<63)|(0x07UL))
#define INTERRUPT_ECALL_USER   ((0x00UL<<63)|(0x08UL))
#define INTERRUPT_ECALL_SYSTEM ((0x00UL<<63)|(0x09UL))

using InterruptRoutine = uint64 (*)(uint64, uint64, uint64, uint64);

class Kernel
{
public:

    static TCB* master;

    static const uint64 irrNum = 0x80;
    static InterruptRoutine irr[irrNum];

    static bool inInterrupt;

    static void init();
    static void terminate();

    // pop sstatus.spp and sstatus.spie bits (has to be a non inline function)
    static void popSppSpie();

    // read register scause
    static uint64 r_scause();

    // write register scause
    static void w_scause(uint64 scause);

    // read register sepc
    static uint64 r_sepc();

    // write register sepc
    static void w_sepc(uint64 sepc);

    // read register stvec
    static uint64 r_stvec();

    // write register stvec
    static void w_stvec(uint64 stvec);

    // read register stval
    static uint64 r_stval();

    // write register stval
    static void w_stval(uint64 stval);

    enum BitMaskSip
    {
        SIP_SSIP = (1 << 1),
        SIP_STIP = (1 << 5),
        SIP_SEIP = (1 << 9),
    };

    // mask set register sip
    static void ms_sip(uint64 mask);

    // mask clear register sip
    static void mc_sip(uint64 mask);

    // read register sip
    static uint64 r_sip();

    // write register sip
    static void w_sip(uint64 sip);

    enum BitMaskSstatus
    {
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8),
    };

    // mask set register sstatus
    static void ms_sstatus(uint64 mask);

    // mask clear register sstatus
    static void mc_sstatus(uint64 mask);

    // read register sstatus
    static uint64 r_sstatus();

    // write register sstatus
    static void w_sstatus(uint64 sstatus);

    // read register a0
    static uint64 r_a0();

    // write register a0
    static void w_a0(uint64 a0);

    // read register a1
    static uint64 r_a1();

    // write register a1
    static void w_a1(uint64 a1);

    // read register ra
    static uint64 r_ra();

    // supervisor trap
    static void supervisorTrap();

private:

    // supervisor trap handler
    static void handleSupervisorTrap(uint64 a0, uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleMemAlloc(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleMemFree(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleThreadCreate(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleThreadExit(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleThreadDispatch(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleJoin(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleTimedJoin(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleJoinAll(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleSemOpen(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleSemClose(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleSemWait(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleSemSignal(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleSemTimedWait(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleSemTryWait(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleTimeSleep(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleGetc(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handlePutc(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleSend(uint64 a1, uint64 a2, uint64 a3, uint64 a4);

    static uint64 handleReceive(uint64 a1, uint64 a2, uint64 a3, uint64 a4);
};

inline uint64 Kernel::r_scause()
{
    uint64 volatile scause;
    __asm__ volatile ("csrr %[scause], scause" : [scause] "=r"(scause));
    return scause;
}

inline void Kernel::w_scause(uint64 scause)
{
    __asm__ volatile ("csrw scause, %[scause]" : : [scause] "r"(scause));
}

inline uint64 Kernel::r_sepc()
{
    uint64 volatile sepc;
    __asm__ volatile ("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline void Kernel::w_sepc(uint64 sepc)
{
    __asm__ volatile ("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

inline uint64 Kernel::r_stvec()
{
    uint64 volatile stvec;
    __asm__ volatile ("csrr %[stvec], stvec" : [stvec] "=r"(stvec));
    return stvec;
}

inline void Kernel::w_stvec(uint64 stvec)
{
    __asm__ volatile ("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}

inline uint64 Kernel::r_stval()
{
    uint64 volatile stval;
    __asm__ volatile ("csrr %[stval], stval" : [stval] "=r"(stval));
    return stval;
}

inline void Kernel::w_stval(uint64 stval)
{
    __asm__ volatile ("csrw stval, %[stval]" : : [stval] "r"(stval));
}

inline void Kernel::ms_sip(uint64 mask)
{
    __asm__ volatile ("csrs sip, %[mask]" : : [mask] "r"(mask));
}

inline void Kernel::mc_sip(uint64 mask)
{
    __asm__ volatile ("csrc sip, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Kernel::r_sip()
{
    uint64 volatile sip;
    __asm__ volatile ("csrr %[sip], sip" : [sip] "=r"(sip));
    return sip;
}

inline void Kernel::w_sip(uint64 sip)
{
    __asm__ volatile ("csrw sip, %[sip]" : : [sip] "r"(sip));
}

inline void Kernel::ms_sstatus(uint64 mask)
{
    __asm__ volatile ("csrs sstatus, %[mask]" : : [mask] "r"(mask));
}

inline void Kernel::mc_sstatus(uint64 mask)
{
    __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Kernel::r_sstatus()
{
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void Kernel::w_sstatus(uint64 sstatus)
{
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

inline uint64 Kernel::r_a0()
{
    uint64 volatile a0_val;
    __asm__ volatile ("mv %[a0_val], a0" : [a0_val] "=r"(a0_val));
    return a0_val;
}

inline void Kernel::w_a0(uint64 a0_val)
{
    __asm__ volatile ("mv a0, %[a0_val]" : : [a0_val] "r"(a0_val));
}

inline uint64 Kernel::r_a1()
{
    uint64 volatile a1_val;
    __asm__ volatile ("mv %[a1_val], a1" : [a1_val] "=r"(a1_val));
    return a1_val;
}

inline void Kernel::w_a1(uint64 a1_val)
{
    __asm__ volatile ("mv a1, %[a1_val]" : : [a1_val] "r"(a1_val));
}

inline uint64 Kernel::r_ra()
{
    uint64 volatile ra_val;
    __asm__ volatile ("mv %[ra_val], ra" : [ra_val] "=r"(ra_val));
    return ra_val;
}

#endif // __KERNEL__HPP__
