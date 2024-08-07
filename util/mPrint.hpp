#ifndef _M_PRINT_HPP_
#define _M_PRINT_HPP_

#include "../h/syscall_c.h"

#define FACTOR_24 1677722ul
#define FACTOR_32 429496730ul

typedef unsigned long uint64;

extern uint64 mLockPrint;

extern "C" uint64 m_copy_and_swap(uint64 &lock, uint64 expected, uint64 desired);

#define MLOCK()     while (m_copy_and_swap(mLockPrint, 0, 1)) thread_dispatch()
#define MUNLOCK()   while (m_copy_and_swap(mLockPrint, 1, 0))

void mPrintString_nonatom(char const *string);

inline void mPrintString(char const *string)
{
    MLOCK();
    mPrintString_nonatom(string);
    MUNLOCK();
}

char* mGetString(char *buf, int max);

int mStringToInt(const char *s);

void mPrintInt_nonatom(int xx, int base=10, int sgn=0);

inline void mPrintInt(int xx, int base=10, int sgn=0)
{
    MLOCK();
    mPrintInt_nonatom(xx, base, sgn);
    MUNLOCK();
}

#endif // _M_PRINT_HPP_

