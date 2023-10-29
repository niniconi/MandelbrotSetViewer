#ifndef __THREAD_H
#define __THREAD_H

#include <pthread.h>

#define LOCK 0
#define UNLOCK 1

extern int thread_count;
extern int calculate_progress;

int init_thread(void (*process)(int*));
void init_other_thread(void (*listen)(void),void (*check_progress)(void),void (*render)(void));
void mutex_thread_lock(int option);
void exit_thread();
void destroy_threads();
void join_threads();
void pSleep(unsigned int time);

#endif
