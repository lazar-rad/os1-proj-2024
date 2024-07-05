.global syscall
.type syscall, @function

syscall:
    ecall
    ret