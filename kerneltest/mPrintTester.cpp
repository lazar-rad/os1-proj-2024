#include "../lib/hw.h"

#include "../kerneltest/mPrintTester.hpp"
#include "../kerneltest/testers.hpp"

#include "../util/mPrint.hpp"
#include "../util/mPrintUtils.hpp"

void mPrintTester::verifyDivision()
{
#if USE_MPRINT_TESTER == 1

    for (uint i = 0; i != ~0u; i++)
    {
        if ((i & 0xfffff) == 0) log("i = ", i);

        uint64 qt = (FACTOR_32 * i) >> 32;
        long rt = i - 10 * qt;
        while (rt < 0) qt--, rt+=10;

        uint64 qo = i / 10;
        long ro = i % 10;

        if ((qo != qt) || (ro != rt))
        {
            log("***** error at ", i);
            log("* qo = ", (uint)qo);
            log("* qt = ", (uint)qt);
            log("* ro = ", (uint)ro);
            log("* rt = ", (uint)rt, 10, 1);
            break;
        }
    }   

#endif
}