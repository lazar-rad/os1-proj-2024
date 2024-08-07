#ifndef __M__PRINTING__UTILS__HPP__
#define __M__PRINTING__UTILS__HPP__

#include "../lib/hw.h"
#include "../util/mPrint.hpp"
#include "../util/arithmetics.hpp"

#define endl putc('\n')

inline void mPrintPointer_nonatom(const void* ptr)
{
    putc('0');
    putc('x');
    mPrintInt_nonatom((uint64)ptr, 16);
}

inline void mPrintPointer(const void* ptr)
{
    MLOCK();
    mPrintPointer_nonatom(ptr);
    MUNLOCK();
}

inline void mPrintFractional_nonatom(const Fractional& frac)
{
    mPrintInt_nonatom(frac.whole);
    putc('.');
    for (uint64 i = 0; i < frac.zeros; i++) putc('0');
    mPrintInt_nonatom(frac.partial);
}

inline void mPrintFractional(const Fractional& frac)
{
    MLOCK();
    mPrintFractional_nonatom(frac);
    MUNLOCK();
}

inline void mPrintPercent_nonatom(const Fractional& percent)
{
    mPrintFractional_nonatom(percent);
    putc('%');
}

inline void mPrintPercent(const Fractional& percent)
{
    MLOCK();
    mPrintPercent_nonatom(percent);
    MUNLOCK();
}

inline void mPrintComplex_nonatom(const Complex& z, int base = 10)
{
    mPrintInt_nonatom(z.real, base);
    mPrintString_nonatom(" + ");
    mPrintInt_nonatom(z.imag, base);
    putc('i');
}

inline void mPrintComplex(const Complex& z, int base = 10)
{
    MLOCK();
    mPrintComplex_nonatom(z, base);
    MUNLOCK();
}

inline void log(const char* string, int xx, int base = 10, int sgn = 0)
{
    MLOCK();
    mPrintString_nonatom(string);
    mPrintInt_nonatom(xx, base, sgn);
    endl;
    MUNLOCK();
}

#ifdef __cplusplus
inline void log(const char* string)
{
    MLOCK();
    mPrintString_nonatom(string);
    endl;
    MUNLOCK();
}

inline void log(const char* string, const Fractional& frac, bool percent = 0)
{
    MLOCK();
    mPrintString_nonatom(string);
    if (percent) mPrintPercent_nonatom(frac);
    else mPrintFractional_nonatom(frac);
    endl;
    MUNLOCK();
}

inline void log(const char* string, const void* p)
{
    MLOCK();
    mPrintString_nonatom(string);
    mPrintPointer_nonatom(p);
    endl;
    MUNLOCK();
}

inline void log(const char* string, const Complex& z)
{
    MLOCK();
    mPrintString_nonatom(string);
    mPrintComplex_nonatom(z);
    endl;
    MUNLOCK();
}
#endif

#endif // __M__PRINTING__UTILS__HPP__