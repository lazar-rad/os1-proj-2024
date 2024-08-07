#include "../lib/hw.h"

#include "../h/kSemaphore.hpp"
#include "../kerneltest/kSemaphoreTester.hpp"
#include "../kerneltest/testers.hpp"

#include "../util/mPrint.hpp"
#include "../util/mPrintUtils.hpp"


void __attribute__((weak)) kSemaphoreTester::test() { }

void __attribute__((weak)) kSemaphoreTester::test_timedWait() { }