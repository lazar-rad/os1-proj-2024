#ifndef __CONSOLE__H__
#define __CONSOLE__H__

#ifdef __cplusplus
extern "C" {
#endif

    void __putc(char chr);

    char __getc();

    void console_handler();

#ifdef __cplusplus
}
#endif

#endif // __CONSOLE__H__