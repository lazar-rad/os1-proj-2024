#ifndef __TCB__TESTER__HPP__
#define __TCB__TESTER__HPP__

#include "../h/tcb.hpp"

class TCBTester
{
public:
    static void probe(const TCB* tcb);
    static void test();
    static void dummy(void* p);
};

#endif // __TCB__TESTER__HPP__