#include "../lib/hw.h"

#include "../h/memoryManager.hpp"
#include "../kerneltest/memoryManagerTester.hpp"
#include "../kerneltest/testers.hpp"

#include "../util/mPrint.hpp"
#include "../util/mPrintUtils.hpp"
#include "../util/arithmetics.hpp"
#include "../util/memoryManagerUtils.hpp"

void __attribute__((weak)) MemoryManagerTester::test() { }

void MemoryManagerTester::usageDiagnostics(int base)
{
#if USE_MEMORY_MANAGER_TESTER == 1
    size_t heapSize = (uint64)HEAP_END_ADDR - (uint64)HEAP_START_ADDR;
    mPrintString("Memory usage diagnostics\n");
    log("  heap size: ", heapSize, base);
    log("  free  : ", Fractional(totalSize(MemoryManager::freeMemoryHead)*100, heapSize, 2), 1);
    log("  system: ", Fractional(totalSize(MemoryManager::systemMemoryHead)*100, heapSize, 2), 1);
    log("  user  : ", Fractional(totalSize(MemoryManager::userMemoryHead)*100, heapSize, 2), 1);
#endif
    (void)base;
}