#ifndef __MANDELBROTSET_H
#define __MANDELBROTSET_H

#include <stdio.h>

#ifdef _WIN32

#elif __linux__
    #include "Linux/thread.h"
    #include "Linux/listenKey.h"
#else
    #error "Unknown Opreating System"
#endif

#ifdef DEBUG
    #define clean()
    #define restpos()
    #define hidecur()
    #define showcur()
    #define debug(label,format,args...) \
        do{ \
            printf("[DEBUG][%s][%s]:",__FUNCTION__,label); \
            printf(format,##args); \
            printf("\n"); \
        }while(0)
#else
    #define clean() printf("\e[2J")
    #define restpos() printf("\e[0;0H")
    #define hidecur() printf("\e[?25l")
    #define showcur() printf("\e[?25h")
    #define debug(label,format,args...)
#endif

#endif
