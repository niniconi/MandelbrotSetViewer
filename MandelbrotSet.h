#ifndef __MANDELBROTSET_H
#define __MANDELBROTSET_H

#include <stdio.h>

#include "Linux/thread.h"
#include "Linux/listenKey.h"

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
