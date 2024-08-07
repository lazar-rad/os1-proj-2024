#include "mPrint.hpp"

uint64 mLockPrint = 0;

void mPrintString_nonatom(char const *string)
{
    while (*string != '\0')
    {
        putc(*string);
        string++;
    }
}

char* mGetString(char *buf, int max)
{
    MLOCK();
    int i, cc;
    char c;

    for(i=0; i+1 < max; ){
        cc = getc();
        if(cc < 1)
            break;
        c = cc;
        buf[i++] = c;
        if(c == '\n' || c == '\r')
            break;
    }
    buf[i] = '\0';

    MUNLOCK();
    return buf;
}

int mStringToInt(const char *s)
{
    int n;

    n = 0;
    while ('0' <= *s && *s <= '9')
        n = n * 10 + *s++ - '0';
    return n;
}

int mConvertInt(char* buf, int xx, int base, int sgn)
{
    static const char digits[] = "0123456789ABCDEF";
    int i, neg;
    uint x;

    neg = 0;
    if (sgn && xx < 0) {
        neg = 1;
        x = -xx;
    } else {
        x = xx;
    }

    if (base < 2 || base > 16) return 0;

    i = 0;
    switch (base)
    {
        case 10:
            {
                uint64 xl = x;
                do {
                    uint64 q = (FACTOR_32 * xl) >> 32;
                    long r = xl - 10 * q;
                    while (r < 0) q--, r+=10;
                    buf[i++] = digits[r];
                    xl = q;
                } while(xl != 0);
            }
            break;

        case 16:
            do {
                buf[i++] = digits[x & 0x0f];
            } while((x >>= 4) != 0);
            break;

        case 8:
            do {
                buf[i++] = digits[x & 0x07];
            } while((x >>= 3) != 0);
            break;

        case 4:
            do {
                buf[i++] = digits[x & 0x03];
            } while((x >>= 2) != 0);
            break;

        case 2:
            do {
                buf[i++] = digits[x & 0x01];
            } while((x >>= 1) != 0);
            break;

        default:
            do {
                buf[i++] = digits[x % base];
            } while((x /= base) != 0);
            break;
    }
    if(neg)
        buf[i++] = '-';

    return i;
}

void mPrintInt_nonatom(int xx, int base, int sgn)
{
    char buf[16];
    int i = mConvertInt(buf, xx, base, sgn);
    
    while(--i >= 0)
    {
        putc(buf[i]);
    }
}