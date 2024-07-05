.global _ZN3TCB13contextSwitchEPNS_7ContextES1_
.type _ZN3TCB13contextSwitchEPNS_7ContextES1_, @function

_ZN3TCB13contextSwitchEPNS_7ContextES1_:
    sd ra, 0*8(a0)
    sd sp, 1*8(a0)
    .irp index 0,1,2,3,4,5,6,7,8,9,10,11
    sd s\index, (\index+2)*8(a0)
    .endr

    ld ra, 0*8(a1)
    ld sp, 1*8(a1)
    .irp index 0,1,2,3,4,5,6,7,8,9,10,11
    ld s\index, (\index+2)*8(a1)
    .endr

    ret