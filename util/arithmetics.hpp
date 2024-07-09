#ifndef __ARITHMETICS__HPP__
#define __ARITHMETICS__HPP__

#include "../lib/hw.h"

inline uint64 countDigits(uint64 x)
{
    uint64 i = 1;
    while (x /= 10) i++;
    return i;
}

struct Fractional
{
    uint64 whole;
    uint64 partial;
    uint64 zeros;
    
    Fractional(uint64 dividend, uint64 divisor, uint64 decimalPlaces)
    {
        uint64 f = 1;
        for (uint64 i = 0; i < decimalPlaces; i++) f *= 10;
        uint64 D = dividend * f / divisor;
        whole = D / f;
        partial = D % f;
        zeros = decimalPlaces - countDigits(partial);
    }
};

struct Complex
{
    uint64 real;
    uint64 imag;

    Complex() : real(0), imag(0) { }
    Complex(uint64 re, uint64 im) : real(re), imag(im) { }
};

#endif // __ARITHMETICS__HPP__