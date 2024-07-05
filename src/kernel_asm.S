.global _ZN6Kernel14supervisorTrapEv
.type _ZN6Kernel14supervisorTrapEv, @function

.align 4

_ZN6Kernel14supervisorTrapEv:
#     csrw sscratch, t1
#     ld t1, _ZN3TCB7runningE
#     ld t1, 4*8(t1)
#     bnez t1, .L1
#     ld t1, _ZN3TCB7runningE
#     sd sp, 7*8(t1)
#     ld sp, 8*8(t1)
# .L1:
#     csrr t1, sscratch

    addi sp, sp, -240
    .irp index 0,1,2,3,4,5,6,7,8,9
    sd x\index, \index * 8(sp)
    .endr
    .irp index 12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, (\index-2) * 8(sp)
    .endr

    call _ZN6Kernel20handleSupervisorTrapEmmmmm

    .irp index 0,1,2,3,4,5,6,7,8,9
    ld x\index, \index * 8(sp)
    .endr
    .irp index 12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, (\index-2) * 8(sp)
    .endr
    addi sp, sp, 240

#     csrw sscratch, t1
#     ld t1, _ZN3TCB7runningE
#     ld t1, 4*8(t1)
#     bnez t1, .L2
#     ld t1, _ZN3TCB7runningE
#     sd sp, 8*8(t1)
#     ld sp, 7*8(t1)
# .L2:
#     csrr t1, sscratch

    sret



.global _ZN6Kernel10popSppSpieEv
.type _ZN6Kernel10popSppSpieEv, @function

_ZN6Kernel10popSppSpieEv:
    csrw sepc, ra
    sret

.global _ZN6Kernel9terminateEv
.type _ZN6Kernel9terminateEv, @function

_ZN6Kernel9terminateEv:
    li t1, 0x5555
    li t2, 0x100000
    sh t1, 0(t2)
    ret