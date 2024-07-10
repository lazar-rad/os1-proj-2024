#ifndef __MEM__H__
#define __MEM__H__

#include "hw.h"

/*
 * The functions are intended to be used in supervisor mode, and they are not thread-safe
 */

#ifdef __cplusplus
    extern "C" {
#endif

    void* __mem_alloc(size_t size);

    int __mem_free(void* ptr);


#ifdef __cplusplus
    }
#endif

#endif // __MEM__H__