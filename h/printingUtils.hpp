#ifndef __PRINTING__UTILS__HPP__
#define __PRINTING__UTILS__HPP__

#include "../test/printing.hpp"

#define endl do { printString("\n"); } while (0)

inline void log(const char* string, int xx, int base = 10, int sgn = 0)
{
    printString(string);
    printInt(xx, base, sgn);
    endl;
}



#endif // __PRINTING__UTILS__HPP__