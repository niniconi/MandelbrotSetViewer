#ifndef __SIG_H
#define __SIG_H
#include <signal.h>

#define regsiter_quit(quit) signal(SIGINT, quit)
#define kill_out() kill(0,SIGINT)

#endif
