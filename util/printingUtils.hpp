#ifndef __PRINTING__UTILS__HPP__
#define __PRINTING__UTILS__HPP__

#include "../lib/hw.h"
#include "../test/printing.hpp"
#include "../util/arithmetics.hpp"

#define endl do { printString("\n"); } while (0)

inline void printPointer(const void* ptr) { printString("0x"); printInt((uint64)ptr, 16); }

inline void printFractional(const Fractional& frac)
{
    printInt(frac.whole);
    printString(".");
    for (uint64 i = 0; i < frac.zeros; i++) printString("0");
    printInt(frac.partial);
}

inline void printPercent(const Fractional& percent)
{
    printFractional(percent);
    printString("%");
}

inline void printComplex(const Complex& z, int base = 10)
{
    printInt(z.real, base);
    printString(" + ");
    printInt(z.imag, base);
    printString("i");
}

inline void log(const char* string, int xx, int base = 10, int sgn = 0)
{
    printString(string);
    printInt(xx, base, sgn);
    endl;
}

#ifdef __cplusplus
inline void log(const char* string)
{
    printString(string);
    endl;
}

inline void log(const char* string, const Fractional& frac, bool percent = 0)
{
    printString(string);
    if (percent) printPercent(frac);
    else printFractional(frac);
    endl;
}

inline void log(const char* string, const void* p)
{
    printString(string);
    printPointer(p);
    endl;
}

inline void log(const char* string, const Complex& z)
{
    printString(string);
    printComplex(z);
    endl;
}
#endif

#endif // __PRINTING__UTILS__HPP__