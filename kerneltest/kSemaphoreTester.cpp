#include "../lib/hw.h"

#include "../h/kSemaphore.hpp"
#include "../kerneltest/kSemaphoreTester.hpp"
#include "../kerneltest/testers.hpp"

#include "../test/printing.hpp"
#include "../util/printingUtils.hpp"


void __attribute__((weak)) kSemaphoreTester::test() { }

void __attribute__((weak)) kSemaphoreTester::test_timedWait() { }